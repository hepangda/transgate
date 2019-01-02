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

#include "fastcgi_manager.h"

namespace tg {

void FastcgiManager::delegate(FastcgiProvider provider) {
  auto ret = std::make_shared<FastcgiProvider>(provider);
  ret->set_gateway(std::make_shared<TcpClient>(InetAddress(ret->config()->gateway(), ret->config()->port())));
  ret->gateway()->connect();
  providing_map_.emplace(ret->gateway()->fd(), ret);
  epoll_.add(*ret->gateway(), ETEOWriteable());
}

void FastcgiManager::poll() {
  EpollEventResult event_result{ConfigProvider::get().evloopEpollEvents()};

  constexpr int kImmediately = 0;
  epoll_.waitUntil(event_result, kImmediately);

  for (int i = 0; i < event_result.size(); i++) {
    auto &it = event_result[i];
    int id = it.event_fd();

    if (it.check(kEPReadable)) {

    }

    if (it.check(kEPWriteable)) {

    }
  }
}

}
