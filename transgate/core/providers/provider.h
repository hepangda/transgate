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

#ifndef TRANSGATE_PROVIDER_H
#define TRANSGATE_PROVIDER_H

#include "../../http/http_request.h"
#include "../../http/http_types.h"
#include "../../net/write_loop.h"

namespace tg {

class Provider {
 public:
  Provider(std::shared_ptr<HttpRequest> request_, std::shared_ptr<WriteLoop> write_loop_) :
      request_(std::move(request_)), write_loop_(std::move(write_loop_)) {}
  virtual void provide() = 0;
 protected:
  int writeHead(int ver_major, int ver_minor, HttpStatusCode code);
  int writeItem(const char *key, const char *value);
  int writeItemConnection(bool keep_alive);
  int writeItemDate();
  int writeCRLF();

  int provideError();
  int regularProvide(bool keep_alive, int content_length, const char *mime);
  int regularProvide(bool keep_alive, int content_length);

  const char *adaptMIME() const;
  constexpr const char *defaultMIME() const { return "text/html"; }
  const char *mimeTable(const char *ext, size_t n) const;
  bool isFilename(char x) const;

  std::shared_ptr<HttpRequest> request_;
  std::shared_ptr<WriteLoop> write_loop_;
};

}

#endif // TRANSGATE_PROVIDER_H
