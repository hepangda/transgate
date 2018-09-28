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

#ifndef TRANSGATE_HTTP_PARSER_H
#define TRANSGATE_HTTP_PARSER_H

#include <memory>

#include "../base/noncopyable.h"
#include "../utils/buffer.h"

namespace tg {

class HttpRequest;

class HttpParser: public Noncopyable {
 public:
  HttpParser(std::shared_ptr<Buffer> stream, std::shared_ptr<HttpRequest> request)
      : stream_(std::move(stream)), request_(std::move(request)) {}

  enum Errors {
    kEFine,
    kELowBufferLength,
    kEInvalidMethod,
    kEInvalidUri,
    kEInvalidVersion,
    kEInvalidContentLength,
    kEUnsupportedVersion,
    kECRLF,
    kEInvalidHeader,
    kEUnrecognizedChar,
    kEUnexceptedEnd,
    kEEntityTooLarge,
  };

  Errors doParse();
 private:
  std::shared_ptr<Buffer> stream_;
  std::shared_ptr<HttpRequest> request_;
};

}

#endif //TRANSGATE_HTTP_PARSER_H
