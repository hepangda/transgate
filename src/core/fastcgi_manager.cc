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

    if (providing_map_.count(id) == 0) {
      epoll_.remove(EpollEvent(id));
      continue;
    }

    const auto &obj = providing_map_[id];

    if (it.check(kEPReadable)) {
      switch (obj->stage()) {
      case kFSRecving:
        obj->gateway()->read(obj->buffer());
        obj->consume();
        if (obj->parse_end()) {
          epoll_.modify(EpollEvent(id, ETEOReadable()));
          obj->set_stage(kFSForwarding);
        } else if (obj->isHeaderEnds()) {
          epoll_.modify(EpollEvent(id, ETEOReadable()));
          obj->set_stage(kFSParsing);
        } else {
          epoll_.modify(EpollEvent(id, ETEOReadable()));
          break;
        }
      case kFSParsing:
        if (obj->stage() == kFSParsing) {
          obj->parse();
          obj->set_stage(kFSForwarding);
        } else if (obj->stage() == kFSForwarding) {
        } else {
          providing_map_.erase(id);
          epoll_.remove(it);
          break;
        }
      case kFSForwarding:
        obj->gateway()->read(obj->buffer());
        obj->consume();
        obj->provide();
        if (obj->forward_end()) {
          providing_map_.erase(id);
          epoll_.remove(it);
        } else {
          epoll_.modify(EpollEvent(id, ETEOReadable()));
        }
      default:
        break;
      }
    } else if (it.check(kEPWriteable)) {
      switch (obj->stage()) {
      case kFSBegin:
        obj->set_stage(kFSConnected);
      case kFSConnected:
        obj->makeRequest();
        if (obj->stage() == kFSConnected) {
          obj->set_stage(kFSSent);
        } else {
          providing_map_.erase(id);
          epoll_.remove(it);
          break;
        }
      case kFSSent:
        obj->buffer()->clear();
        obj->set_stage(kFSRecving);
        epoll_.modify(EpollEvent(id, ETEOReadable()));
      default:
        break;
      }
    }
  }
}

}
