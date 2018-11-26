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

#ifndef TRANSGATE_TG_H
#define TRANSGATE_TG_H


#include "../net/thread_pool.h"
#include "../net/tcp_server.h"
#include "../net/epoll.h"
#include "user_manager.h"

namespace tg {

class Transgate {
 public:
  Transgate();
  void registerAccept();
  void run();
 private:
  UserManager usrmgr_;
  TcpServer server_;
  Epoll epoll_;
};

}

#endif // TRANSGATE_TG_H
