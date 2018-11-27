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
#include <signal.h>
#include <thread>
#include <vector>

#include "../protocol/http_parser.hpp"
#include "../protocol/http_response.hpp"
#include "../utils/debug.hpp"
#include "transgate.h"

using namespace tg;

void Transgate::consume(Transgate *self, ClientSocket clt) {
  DPRINT("=====START=====\n");

  ByteBuffer buf(1000);
  buf.clean();
  auto len = clt.read(buf, 1000);

  tdbc(buf.pointer<char>());
  auto req = HttpRequestParser(buf)(len);

  DPRINT("[PARSED]\n");

  auto rep = HttpResponse(&req);
  DPRINT("[GENERATED]\n");

  rep.sendTo(clt);
  DPRINT("[WRITED]\n");

  DPRINT("=====E N D=====\n");

  if (req.getStatus() == HttpRequestStatus::KEEP_ALIVE) {
    self->_epl.add({clt, EpollEventType::READABLE | EpollEventType::ONESHOT});
  } else {
    clt.close();
  }
}

void Transgate::newconn(Transgate *self) {
  auto clt = self->_servsock.accept();
  self->_epl.add({clt, EpollEventType::READABLE | EpollEventType::ONESHOT});
}

void Transgate::rdhup(Socket skt) { skt.close(); }

void Transgate::run() {
  _servsock.setReusePortVar();
  _servsock.bindAndListen();
  _epl.add({_servsock, EpollEventType::READABLE});

  for (;;) {
    auto evts = _epl.wait();
    using Type = EpollEventType;
    for (auto &i : evts) {
      if (i == _servsock) {
        newconn(this);
      } else if (i.checkType({Type::READABLE})) {
        if (i.checkType({Type::SOCKET_CLOSED, Type::ERROR})) {
          rdhup(i.getFd());
        } else {
          consume(this, i.getFd());
        }
      }
    }
  }
}

const char *tg::config::Http::_desc_err = "Internal Server Error";
std::unordered_map<int16_t, std::string> tg::config::Http::_desc;
std::unordered_map<std::string, std::string> tg::config::Http::_conttype;
std::string tg::config::Http::_conttype_any;
std::vector<tg::config::Sites::_Site> tg::config::Sites::_cont;
std::regex tg::config::Fastcgi::reCgi;
std::shared_ptr<char> tg::config::Fastcgi::cgiAddr;
unsigned int tg::config::Fastcgi::port;

int main() {
  signal(SIGPIPE, SIG_IGN);
  ConfigLoader::load();
  std::vector<std::thread> tds;
  for (int i = 0; i < std::thread::hardware_concurrency() - 1; i++) {
    tds.emplace_back(std::thread([] {
      Transgate app;
      app.run();
    }));
  }
  Transgate tr;

  tr.run();
}