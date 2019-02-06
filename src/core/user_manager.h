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

#include "../net/epoll.h"
#include "user.h"
#include "fastcgi_manager.h"

namespace tg {

class Epoll;

class UserManager {
 public:
  explicit UserManager(Epoll &epoll): epoll_(epoll) {}

  bool contains(int id) { return table_.count(id) > 0; }

  int delegate(std::unique_ptr<User> &user, EpollEventType type);
  int delegate(std::unique_ptr<User> &&user, EpollEventType type);
  void activate(int id);
  void release(int id);

  void adapt(int id);
  void eliminate(long time);

  void doReadable(int id, long time);
  void doWriteable(int id, long time);

  void fcgiPoll() { fcgimgr_.poll(); }
 private:
  Epoll &epoll_;
  FastcgiManager fcgimgr_;
  std::unordered_map<int, std::unique_ptr<User>> table_;
};

}

#endif // TRANSGATE_USER_MANAGER_H
