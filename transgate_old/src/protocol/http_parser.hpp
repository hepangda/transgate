//  Copyright 2018 hepangda
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#pragma once
// #include <libgo/coroutine.h>
#include <algorithm>
#include <cctype>
#include <numeric>

#include "../http_parser/http_parser.h"
#include "../utils/bytes.hpp"
#include "../utils/debug.hpp"
#include "http_request.hpp"

namespace tg {

class HttpRequestParser {
 private:
  http_parser _parser;
  http_parser_settings _settings;
  HttpRequest _result;
  ByteBuffer &_buf;

  static int cbMessageBegin(http_parser *parser) { return 0; }

  static int cbMessageComplete(http_parser *parser) {
    HttpRequest *req = reinterpret_cast<HttpRequest *>(parser->data);
    return 0;
  }

  static int cbHeadersComplete(http_parser *parser) {
    HttpRequest *req = reinterpret_cast<HttpRequest *>(parser->data);
    req->setMethod(HttpMethod(parser->method));

    if (parser->http_major != 1 || parser->http_minor != 1) {
      req->setStatus(HttpRequestStatus::HTTP_VERSION_NOT_SUPPORTED);
      return 0;
    }

    if (auto str = req->getFieldValue("Connection"); str != nullptr) {
      constexpr auto sz = sizeof("close");
      if (str->size() == sz &&
          strncasecmp(str->c_str(), "close", str->size())) {
        req->setStatus(HttpRequestStatus::KEEP_ALIVE);
      }
    } else {
      req->setStatus(HttpRequestStatus::KEEP_ALIVE);
    }

    return 0;
  }

  static int cbHeaderField(http_parser *parser, const char *at, size_t length) {
    HttpRequest *req = reinterpret_cast<HttpRequest *>(parser->data);
    req->setReverseKey({at, length});
    return 0;
  }

  static int cbHeaderValue(http_parser *parser, const char *at, size_t length) {
    HttpRequest *req = reinterpret_cast<HttpRequest *>(parser->data);
    auto key = req->getReverseKey();
    req->pushField(std::string(key.data(), key.length()),
                   std::string(at, length));
    return 0;
  }

  static int cbBody(http_parser *parser, const char *at, size_t length) {
    HttpRequest *req = reinterpret_cast<HttpRequest *>(parser->data);
    std::byte *buf = new std::byte[length];
    std::copy(at, at + length, (char *)buf);
    req->setEntity(buf, length);
    return 0;
  }

  static int cbUrl(http_parser *parser, const char *at, size_t length) {
    HttpRequest *req = reinterpret_cast<HttpRequest *>(parser->data);
    req->setUri({at, length});
    return 0;
  }

 public:
  HttpRequestParser(ByteBuffer &buf) : _buf(buf) {
    http_parser_init(&this->_parser, ::HTTP_REQUEST);
    http_parser_settings_init(&this->_settings);

    this->_parser.data = &this->_result;
    this->_settings.on_message_begin = cbMessageBegin;
    this->_settings.on_message_complete = cbMessageComplete;
    this->_settings.on_headers_complete = cbHeadersComplete;
    this->_settings.on_header_field = cbHeaderField;
    this->_settings.on_header_value = cbHeaderValue;
    this->_settings.on_body = cbBody;
    this->_settings.on_url = cbUrl;
  }

  inline HttpRequest operator()(int len) { return parse(len); }

  inline HttpRequest parse(int len) {
    http_parser_execute(&this->_parser, &this->_settings,
                        this->_buf.pointer<char>(), len);

    if (this->_parser.http_errno != ::HPE_OK) {
      DPRINT("[PRASE ERR] %s: %s\n",
             http_errno_name(http_errno(_parser.http_errno)),
             http_errno_description(http_errno(_parser.http_errno)));
      this->_result.setStatus(HttpRequestStatus::BAD_REQUEST);
    }

    return std::move(this->_result);
  }
};

}  // namespace tg