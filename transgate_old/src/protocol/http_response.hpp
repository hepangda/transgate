/****************************************************************************
 * Copyright 2018 hepangda                                                  *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *     http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/
#pragma once
#include <cstddef>
#include <ctime>
#include <memory>

#include "http_err.hpp"
#include "http_request.hpp"
#include "http_types.hpp"
#include "pfastcgi.hpp"

#include "../config/config.hpp"
#include "../utils/debug.hpp"
#include "../utils/rofile.hpp"
#include "../utils/sockets.hpp"

namespace tg {

class HttpResponse {
 private:
  using FieldContainer = std::unordered_map<std::string, std::string>;

  HttpStatusCode _code;
  FieldContainer _fields;
  size_t _entity_size;
  HttpEntityType _type = HttpEntityType::File;
  std::shared_ptr<ROFile> _entity_file = nullptr;
  std::shared_ptr<std::byte> _entity_byte = nullptr;

  inline std::string getDate() const {
    time_t t = time(nullptr);
    char tmbuf[128];
    strftime(tmbuf, 127, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));

    return std::move(std::string(tmbuf));
  }

 public:
  HttpResponse(HttpRequest *req) {
    struct ResponseError {};
    struct FcgiEnd {};
    try {
      auto fields_conn = req->getFieldValue("Connection");
      auto host_str = req->getFieldValue("Host");
      std::string uri = req->getUri();
      SiteID site = -1;

      _fields["Connection"] = (fields_conn == nullptr) ? "Close" : *fields_conn;
      _fields["Server"] = "Transgate " TRANSGATE_VERSION_STRING;
      _fields["Date"] = getDate();

      if (req->getStatus() != HttpRequestStatus::OK) {
        throw ResponseError();
      }

      if (host_str != nullptr) {
        site = config::Sites::matchHost(*host_str);
      }

      if (site == -1) {
        req->setStatus(HttpRequestStatus::BAD_REQUEST);
        throw ResponseError();
      }

      std::string fulluri = config::Sites::getFullpath(site, uri);
      if (config::Fastcgi::isCgi(fulluri)) {
        auto met = req->getMethod();
        if (met != HttpMethod::GET && met != HttpMethod::POST) {
          req->setStatus(HttpRequestStatus::NOT_IMPLEMENTED);
          DOUT("METHOD DON\'t FIT") ENL;
          throw ResponseError();
        }
        std::shared_ptr<FcgiManager> cgim(new FcgiManagerINET(
            config::Fastcgi::getAddr(), config::Fastcgi::getPort()));
        cgim->beginRequest();
        cgim->sendParams("SCRIPT_FILENAME", fulluri.c_str());
        if (met == HttpMethod::GET) {
          cgim->sendParams("REQUEST_METHOD", "GET");
          cgim->sendParams("CONTENT_TYPE", "text/html");
          cgim->sendParams("CONTENT_LENGTH", "0");
          cgim->endParams();
        } else {
          cgim->sendParams("REQUEST_METHOD", "POST");
          cgim->sendParams("CONTENT_LENGTH",
                           std::to_string(req->entitySize()).c_str());
          cgim->sendParams("CONTENT_TYPE", "application/x-www-form-urlencoded");
          cgim->endParams();
          if (req->entitySize() != 0) {
            FcgiManager::FcgiHeader head(FcgiType::STDIN, req->entitySize());
            cgim->doWrite(&head, sizeof(head));
            int r = cgim->doWrite(req->getEntity(), req->entitySize());
            // DOUT( "CTLEN = " << req->entitySize() << ", WRITE = " << r ) ENL;
            head = {FcgiType::STDIN, 0};
            cgim->doWrite(&head, sizeof(head));
          }
        }

        bool skipp = true;
        for (FcgiPreread head; head = cgim->preRead(), true;) {
          switch (cgim->pkgType(head)) {
            case 1: {
              std::shared_ptr<char> buf(new char[head.contentLength],
                                        std::default_delete<char[]>());
              cgim->readContent(head, buf.get());
              DOUT(buf.get()) ENL;
              if (skipp) {
                char *p = (char *)memmem(buf.get(), head.contentLength,
                                         "\r\n\r\n", 4);

                if (p == NULL) {
                              DOUT("FCGI BAD REQUEST") ENL;
                  req->setStatus(HttpRequestStatus::BAD_REQUEST);
                  throw ResponseError();
                }
                skipp = false;
                p += 4;
                int length = head.contentLength - (p - buf.get());

                _entity_byte.reset(new std::byte[length],
                                   std::default_delete<std::byte[]>());
                _entity_size = length;
                memcpy(_entity_byte.get(), p, length);
              } else {
                int length = head.contentLength + _entity_size;
                std::byte *tmp = new std::byte[length];
                memcpy(tmp, _entity_byte.get(), _entity_size);
                memcpy(tmp + _entity_size, buf.get(), head.contentLength);
                _entity_byte.reset(tmp, std::default_delete<std::byte[]>());
                _entity_size = length;
              }

              break;
            }
            case 0:
              if (head.contentLength == 0) {
                break;
              }

              req->setStatus(HttpRequestStatus::NOT_IMPLEMENTED);
              throw ResponseError();
            case -1:
              throw FcgiEnd();
          }
        }
      } else {
        _entity_file = std::make_shared<ROFile>(fulluri);
      }

      switch (config::Sites::methodAllowed(site, req->getMethod())) {
        case 0:
          req->setStatus(HttpRequestStatus::METHOD_NOT_ALLOWED);
          throw ResponseError();
        case -1:
          req->setStatus(HttpRequestStatus::NOT_IMPLEMENTED);
          throw ResponseError();
        default:
          // do nothing
          break;
      }

      if (_entity_file == nullptr || (!_entity_file->ok())) {
        req->setStatus(HttpRequestStatus::NOT_FOUND);
        throw ResponseError();
      }

      if (config::Sites::isForbidden(site, uri)) {
        req->setStatus(HttpRequestStatus::FORBIDDEN);
        throw ResponseError();
      }

      _code = HttpStatusCode::OK;
      _entity_size = _entity_file->getSize();
      _fields["Content-Type"] = config::Http::getContentType({uri});
      _fields["Content-Length"] = std::to_string(_entity_size);

    } catch (ResponseError) {
      _code = HttpError::transcode(req->getStatus());
      auto errpage = HttpError::spawnErrorStream(_code);
      _type = HttpEntityType::ByteStream;
      _entity_byte = errpage.first;
      _entity_size = errpage.second;
      _fields["Content-Type"] = "text/html";
      _fields["Content-Length"] = std::to_string(_entity_size);
    } catch (FcgiEnd) {
      _code = HttpStatusCode::OK;
      _type = HttpEntityType::ByteStream;
      _fields["Content-Type"] = "text/html";
      _fields["Content-Length"] = std::to_string(_entity_size);
    }
  }

  void sendTo(ClientSocket &skt) {
    std::shared_ptr<char> buffer(new char[16384],
                                 std::default_delete<char[]>());

    int formatted = sprintf(buffer.get(), "HTTP/1.1 %d %s\r\n", int(_code),
                            config::Http::getStatusDesc(int16_t(_code)));

    for (auto &i : _fields) {
      int case_length =
          i.first.length() + i.second.length() + 4;  // colon space cr lf
      if (formatted + case_length > 16384) {
        skt.write(buffer.get(), formatted);
        formatted = 0;
      }
      formatted += sprintf(buffer.get() + formatted, "%s: %s\r\n",
                           i.first.c_str(), i.second.c_str());
    }

    strcpy(buffer.get() + formatted, "\r\n");
    formatted += 2;
    skt.write(buffer.get(), formatted);

    if (_type == HttpEntityType::ByteStream) {
      skt.write(_entity_byte.get(), _entity_size);
    } else {
      skt.write(*_entity_file, nullptr, _entity_size);
    }
  }
};

}  // namespace tg
