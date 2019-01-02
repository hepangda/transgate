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

#ifndef TRANSGATE_FASTCGI_MANAGER_H
#define TRANSGATE_FASTCGI_MANAGER_H

#include <queue>
#include "../net/epoll.h"
#include "providers/fastcgi_provider.h"

namespace tg {

class FastcgiManager {
 public:
  FastcgiManager() = default;
  void delegate(FastcgiProvider provider);
  void poll();

 private:
  std::unordered_map<int, std::shared_ptr<FastcgiProvider>> providing_map_;
  Epoll epoll_;
};

}

#endif // TRANSGATE_FASTCGI_MANAGER_H
