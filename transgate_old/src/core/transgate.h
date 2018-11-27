/****************************************************************************
 * Copyright 2018 hepangda                                                  *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *     http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/

#include "../config/config.hpp"
#include "../protocol/http_parser.hpp"
#include "../protocol/http_response.hpp"
#include "../utils/epoll.hpp"
#include "../utils/sockets.hpp"
namespace tg {

class Transgate {
 private:
  ServerSocket _servsock = {{config::PORT}};
  Epoll _epl;

 public:
  void run();

  static void consume(Transgate *self, ClientSocket clt);
  static void newconn(Transgate *self);
  static void rdhup(Socket skt);
};

}  // namespace tg