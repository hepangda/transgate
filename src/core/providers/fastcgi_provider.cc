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

#include "fastcgi_provider.h"

#include <iostream>

#include "../../fastcgi/fcgifwd.h"

namespace tg {

void FastcgiProvider::makeRequest() {
  static const std::string params[] = {"SCRIPT_FILENAME", "REQUEST_METHOD", "CONTENT_LENGTH", "CONTENT_TYPE"};

  try {
    auto body = FcgiMakeBeginRequestBody(kFRResponder, false);
    auto header = FcgiMakeHeader(kFPTBeginRequest, sizeof(*body), 0, 1);

    auto bytes = buffer_->write(header.get(), sizeof(*header));
    bytes += buffer_->write(body.get(), sizeof(*body));
    gateway()->write(buffer_->readptr(), bytes);
    buffer_->read(bytes);
    bytes = 0;

    auto val = host_config_->wwwroot_str() + request_->uri().toString();
    int length = static_cast<int>(params[0].length() + val.length()) + sizeof(FcgiParamsBody);
    auto pbody = FcgiMakeParams(params[0], val);
    header = FcgiMakeHeader(kFPTParams, length, 0, 1);
    bytes += buffer_->write(header.get(), sizeof(*header));
    bytes += buffer_->write(pbody.get(), length);
    gateway()->write(buffer_->readptr(), bytes);
    buffer_->read(bytes);
    bytes = 0;

    val = request_->method() == kHMGet ? "GET" : "POST";
    length = static_cast<int>(params[1].length() + val.length()) + sizeof(FcgiParamsBody);
    pbody = FcgiMakeParams(params[1], val);
    header = FcgiMakeHeader(kFPTParams, length, 0, 1);
    bytes += buffer_->write(header.get(), sizeof(*header));
    bytes += buffer_->write(pbody.get(), length);
    gateway()->write(buffer_->readptr(), bytes);
    buffer_->read(bytes);
    bytes = 0;

    auto valview = request_->getValue(StringView{"Content-Length"});
    val = valview.size() == 0 ? "0" : valview.toString();
    length = static_cast<int>(params[2].length() + val.length()) + sizeof(FcgiParamsBody);
    pbody = FcgiMakeParams(params[2], val);
    header = FcgiMakeHeader(kFPTParams, length, 0, 1);
    bytes += buffer_->write(header.get(), sizeof(*header));
    bytes += buffer_->write(pbody.get(), length);
    gateway()->write(buffer_->readptr(), bytes);
    buffer_->read(bytes);
    bytes = 0;

    valview = request_->getValue(StringView{"Content-Type"});
    val = valview.size() == 0 ? "application/x-www-form-urlencoded" : valview.toString();
    length = static_cast<int>(params[3].length() + val.length()) + sizeof(FcgiParamsBody);
    pbody = FcgiMakeParams(params[3], val);
    header = FcgiMakeHeader(kFPTParams, length, 0, 1);
    bytes += buffer_->write(header.get(), sizeof(*header));
    bytes += buffer_->write(pbody.get(), length);
    gateway()->write(buffer_->readptr(), bytes);
    buffer_->read(bytes);
    bytes = 0;

    header = FcgiMakeHeader(kFPTParams, 0, 0, 1);
    bytes += buffer_->write(header.get(), sizeof(*header));
    gateway()->write(buffer_->readptr(), bytes);
    buffer_->read(bytes);
    bytes = 0;

    if (request_->content_length() > 0) {
      bytes += buffer_->write(request_->content(), request_->content_length());
      gateway()->write(buffer_->readptr(), bytes);
      buffer_->read(bytes);
    }
  } catch (std::invalid_argument &ex) {
    request_->set_code(kHCRequestEntityTooLarge);
    set_stage(kFSDone);
    provideError();
  }
}

bool FastcgiProvider::isHeaderEnds() {
  enum {
    kNull,
    kCR,
    kCRLF,
    kCRLFCR,
  } state = kNull;
  for (int i = 0; i < out_->readable(); i++) {
    switch (state) {
    case kNull:
      if ((*out_)[i] == '\r') state = kCR;
      break;
    case kCR:
      if ((*out_)[i] == '\n') state = kCRLF;
      break;
    case kCRLF:
      if ((*out_)[i] == '\r') state = kCRLFCR;
      break;
    case kCRLFCR:
      if ((*out_)[i] == '\n') return true;
      break;
    }
  }
  return false;
}

void FastcgiProvider::consume() {
  if (end) return;
  while (buffer_->readable() > sizeof(FcgiHeader)) {
    auto header = FcgiGetHeader(buffer_->readptr());
    if (header->contentLength == 0) {
      end = true;
      buffer_->read(sizeof(FcgiHeader));
      return;
    }

    if (buffer_->readable() >= header->contentLength + sizeof(FcgiHeader)) {
      buffer_->read(sizeof(FcgiHeader));
      if (!isStderr && header->type == kFPTStdout) {
        out_->write(buffer_->readptr(), header->contentLength);
        buffer_->read(header->contentLength + header->paddingLength);
      } else if (header->type == kFPTStderr) {
        if (!isStderr) {
          out_->clear();
          code_ = kHCBadRequest;
          isStderr = true;
        }
        out_->write(buffer_->readptr(), header->contentLength);
        buffer_->read(header->contentLength + header->paddingLength);
      } else {
        buffer_->read(header->contentLength + header->paddingLength);
      }
    } else {
      buffer_->move();
    }
  }
}

void FastcgiProvider::provide() {
  if (!provided) {
    if (isStderr) {
      provideError();
    } else {
      int ret = writeHead(request_->major_version(), request_->minor_version(), code_);
      for (auto &i: fields_) {
        ret += writeItem(i.first.c_str(), i.second.c_str());
      }
//      ret += writeItem("Transfer-Encoding", "chunked");
      ret += writeCRLF();
      write_loop_->appendSend(ret);
    }
    provided = true;
  }

  int amount = std::min(out_->readable(), write_loop_->writeable());
  int ret = write_loop_->write(out_->readptr(), amount);
  out_->read(amount);
  write_loop_->appendSend(ret);
  write_loop_->move();
  doForward();
}

void FastcgiProvider::parse() {
  enum {
    kBegin,
    kKey,
    kColon,
    kWhite,
    kValue,
    kCR,
    kCRLF,
    kDone
  } state = kBegin;

  std::string key, value;
  for (; out_->readable() > 1 && state != kDone;) {
    auto ch = out_->readptr()[0];
    out_->read(1);
    switch (state) {
    case kBegin:
      if (ch == ' ' || ch == '\t' || ch == '\n') break;
      else if (ch == '\r') state = kCR;
      else {
        key += ch;
        state = kKey;
      }
      break;
    case kKey:
      if (ch == ':') state = kColon;
      else key += ch;
      break;
    case kColon:
      if (ch == ' ' || ch == '\t') state = kWhite;
      else {
        value += ch;
        state = kValue;
      }
      break;
    case kWhite:
      if (ch == ' ' || ch == '\t') break;
      else {
        value += ch;
        state = kValue;
      }
      break;
    case kValue:
      if (ch == '\r') state = kCR;
      else value += ch;
      break;
    case kCR:
      if (ch == '\n') state = kCRLF;
      break;
    case kCRLF:
      if (strcasecmp("status", key.c_str()) == 0) {
        int code = std::stoi(value);
        code_ = HttpStatusCode(code);
      } else {
        fields_[key] = value;
        key = value = "";
      }
      if (ch == '\r') {
        state = kDone;
        out_->read(1);
      } else {
        key += ch;
        state = kKey;
      }
    default:
      break;
    }
  }
}

}