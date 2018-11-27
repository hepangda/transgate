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

#ifndef TRANSGATE_USER_MANAGER_H
#define TRANSGATE_USER_MANAGER_H

#include <memory>
#include <unordered_map>
#include "../http/http_user.h"
#include "../net/epoll.h"

namespace tg {

class UserManager {
 public:
  explicit UserManager(Epoll &epoll): epoll_(epoll) {}

  bool contains(int id) { return table_.count(id) > 0; }

  int delegate(std::unique_ptr<HttpUser> &user, EpollEventType type) {
    int ret = user->fd();
    table_.emplace(ret, user.release());
    epoll_.add(*table_[ret], type);
    return ret;
  };

  int delegate(std::unique_ptr<HttpUser> &&user, EpollEventType type) {
    int ret = user->fd();
    table_.emplace(ret, user.release());
    epoll_.add(*table_[ret], type);
    return ret;
  }

  void doReadable(int id) {
    if (!contains(id)) {
      throw std::invalid_argument("`UserManager::doReadable` no such user.");
    }
    table_[id]->onRead();
  }

  void activate(int id, EpollEventType type) {
    if (!contains(id)) {
      throw std::invalid_argument("`UserManager::addEventsOfReadable` no such user.");
    }
    epoll_.modify(*table_[id], type);
  }

  void release(int id) {
    if (!contains(id)) {
      throw std::invalid_argument("`UserManager::release` released the incorrect user.");
    }
    epoll_.remove(*table_[id]);
    table_.erase(id);
  }
 private:
  Epoll &epoll_;
  std::unordered_map<int, std::unique_ptr<HttpUser>> table_;
};

}

#endif // TRANSGATE_USER_MANAGER_H
