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

#include "user_manager.h"

#include "../http/http_user.h"

namespace tg {

int UserManager::delegate(std::unique_ptr<HttpUser> &user, EpollEventType type) {
  int ret = user->fd();
  table_.emplace(ret, user.release());
  epoll_.add(*table_[ret], type);
  return ret;
}

int UserManager::delegate(std::unique_ptr<tg::HttpUser> &&user, tg::EpollEventType type)  {
  int ret = user->fd();
  table_.emplace(ret, user.release());
  epoll_.add(*table_[ret], type);
  return ret;
}

void UserManager::activate(int id, EpollEventType type)  {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::addEventsOfReadable` no such user.");
  }
  epoll_.modify(*table_[id], type);
}

void UserManager::release(int id) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::release` released the incorrect user.");
  }
  epoll_.remove(*table_[id]);
  table_.erase(id);
}

void UserManager::doReadable(int id) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::doReadable` no such user.");
  }
  table_[id]->onRead();
}

}