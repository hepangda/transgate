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
#include "../http/http_user.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

namespace tg {

void Transgate::run() {
  try {
    signal(SIGPIPE, SIG_IGN);
    server_.setVar(TcpServer::ReusePort);
    server_.bindAndListen();
    epoll_.add(EpollEvent(server_, ETEOReadable()));

    // TODO: FIX MAGIC NUMBER
    EpollEventResult event_result{100};

    for (;;) {
      epoll_.wait(event_result);

      for (int i = 0; i < event_result.size(); i++) {
        auto &it = event_result[i];
        int id = it.event_fd();

        if (it.event_fd() == server_.fd()) {
          server_.acceptAll([this](int fd) { usrmgr_.delegate(std::make_unique<HttpUser>(fd), ETEOReadable()); });
          epoll_.modify(server_, ETEOReadable());
        } else if (it.check(kEPSocketClosed)) {
          usrmgr_.release(id);
        } else if (it.check(kEPReadable)) {
          usrmgr_.doReadable(id);
//          usrmgr_.activate(id, ETEOReadable());
          usrmgr_.release(id);
        }
      }
    }

  } catch (std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
}

}
#pragma clang diagnostic pop