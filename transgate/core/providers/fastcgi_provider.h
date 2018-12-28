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

#ifndef TRANSGATE_FASTCGI_PROVIDER_H
#define TRANSGATE_FASTCGI_PROVIDER_H

#include "provider.h"
namespace tg {

class FastcgiProvider : public Provider {
 public:
  FastcgiProvider(const std::shared_ptr<HttpRequest> &request,
                  const std::shared_ptr<WriteLoop> &write_loop,
                  const std::shared_ptr<HostConfig> &host,
                  std::shared_ptr<FcgiConfig> fcgi) :
      Provider(request, write_loop, host), fcgi_config_(std::move(fcgi)) {}
  void provide() final;
 private:
  std::shared_ptr<FcgiConfig> fcgi_config_;
};

}

#endif // TRANSGATE_FASTCGI_PROVIDER_H
