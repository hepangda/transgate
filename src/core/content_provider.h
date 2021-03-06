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

#ifndef TRANSGATE_CONTENT_PROVIDER_H
#define TRANSGATE_CONTENT_PROVIDER_H

#include <memory>

#include "../net/write_loop.h"
#include "../http/http_request.h"
#include "providers/provider.h"
#include "../utils/generic_buffer.h"

namespace tg {

class ContentProvider : Noncopyable {
 public:
  explicit ContentProvider(std::shared_ptr<WriteLoop> wl, std::shared_ptr<HttpRequest> req):
    write_loop_(std::move(wl)), request_(std::move(req)) {}
  void provide(std::shared_ptr<GenericBuffer> interaction_buffer_);
 protected:
  std::unique_ptr<Provider> impl_ = nullptr;
  std::shared_ptr<HttpRequest> request_ = nullptr;
  std::shared_ptr<WriteLoop> write_loop_ = nullptr;
};

}

#endif // TRANSGATE_CONTENT_PROVIDER_H
