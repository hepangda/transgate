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

#ifndef TRANSGATE_STATIC_PROVIDER_H
#define TRANSGATE_STATIC_PROVIDER_H

#include <memory>

#include "provider.h"
#include "../../http/http_request.h"
#include "../../net/write_loop.h"

namespace tg {

class StaticProvider : public Provider {
 public:
  StaticProvider(const std::shared_ptr<HttpRequest> &request_, const std::shared_ptr<WriteLoop> &write_loop_) :
      Provider(request_, write_loop_) {}
  void provide() final;
 private:
};

}

#endif // TRANSGATE_STATIC_PROVIDER_H
