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
#include <iostream>

#include "tg.h"
#include "../net/epoll_event_result.h"
#include "../utils/string_view.h"
#include "../utils/heap_buffer.h"
#include "../http/http_user.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
namespace tg {

Transgate::Transgate() : server_(InetAddress(8090)) {

}

void Transgate::registerAccept() {

}

void Transgate::run() {
  try {
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
          server_.acceptAll([this] (int fd) {
            int nid = usrmgr_.delegate(std::make_unique<HttpUser>(fd));
            usrmgr_.addEventsOfReadable(epoll_, nid);
          });

          epoll_.modify(server_, ETEOReadable());
        } else if (it.check(kEPSocketClosed)) {
          epoll_.remove(it);
        } else if (it.check(kEPReadable)) {
          usrmgr_.doReadable(id);
          TcpSocket client{it.event_fd()};
          HeapBuffer buffer{512};

          client.read(buffer);
          int rt =
              client.write(StringView("HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 260\r\n\r\n<html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"
                                      "<html></html><html></html><html></html><html></html><html></html><html></html>"));
          client.close();
        }

      }
    }

  } catch (std::exception &ex) {
    std::cout << ex.what() << std::endl;
//    std::abort();
  }
}

}
#pragma clang diagnostic pop