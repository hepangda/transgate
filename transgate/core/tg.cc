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

#include "tg.h"

#include <signal.h>

#include "../net/epoll_event_result.h"
#include "../utils/string_view.h"
#include "user.h"
#include "config_provider.h"
#include <iostream>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

namespace tg {

Transgate::Transgate() : server_(InetAddress(ConfigProvider::get().serverPort())), usrmgr_(epoll_) {}

void Transgate::run() {
  try {
    signal(SIGPIPE, SIG_IGN);
    server_.setVar(TcpServer::ReusePort);
    server_.bindAndListen();
    epoll_.add(server_, ETEOReadable());

    EpollEventResult event_result{ConfigProvider::get().evloopEpollEvents()};

    for (;;) {
      epoll_.wait(event_result);

      for (int i = 0; i < event_result.size(); i++) {
        auto &it = event_result[i];
        int id = it.event_fd();

        if (it.event_fd() == server_.fd()) {
          server_.acceptAll([this](int fd) { usrmgr_.delegate(std::make_unique<User>(fd), ETEOReadable()); });
          epoll_.modify(server_, ETEOReadable());
        } else if (it.check(kEPSocketClosed)) {
          usrmgr_.release(id);
        } else if (it.check(kEPReadable)) {
          usrmgr_.doReadable(id);
          usrmgr_.adapt(id);
        } else if (it.check(kEPWriteable)) {
          usrmgr_.doWriteable(id);
          usrmgr_.adapt(id);
        }
      }
    }

  } catch (std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
}

}
#pragma clang diagnostic pop