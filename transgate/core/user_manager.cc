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

#include "user.h"
#include "core_marks.h"

namespace tg {

int UserManager::delegate(std::unique_ptr<User> &user, EpollEventType type) {
  int ret = user->fd();
  table_.emplace(ret, user.release());
  epoll_.add(*table_[ret], type);
  return ret;
}

int UserManager::delegate(std::unique_ptr<User> &&user, EpollEventType type)  {
  int ret = user->fd();
  table_.emplace(ret, user.release());
  epoll_.add(*table_[ret], type);
  return ret;
}

void UserManager::activate(int id) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::activate` no such user.");
  }
  epoll_.modify(*table_[id], table_[id]->type());
}

void UserManager::release(int id) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::release` released the incorrect user.");
  }
  epoll_.remove(*table_[id]);
  table_.erase(id);
}

void UserManager::doReadable(int id, long time) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::doReadable` no such user.");
  }

  std::unique_ptr<User> &user = table_[id];
  user->onRead();
  if (user->get_interaction_buffer()->get_mark(kCMFcgi)) {

  }
  user->touch(time);
}

void UserManager::doWriteable(int id, long time) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::doWriteable` no such user.");
  }
  table_[id]->onWrite();
  table_[id]->touch(time);
}

void UserManager::adapt(int id) {
  if (!contains(id)) {
    throw std::invalid_argument("`UserManager::adapt` no such user.");
  }

  auto &user = *table_[id];
  if (user.closeable()) {
    release(id);
  } else {
    epoll_.modify(*table_[id], table_[id]->type());
  }
}

void UserManager::eliminate(long time) {
  for (auto it = table_.cbegin(); it != table_.cend(); ) {
    if (time - it->second->time_stamp() >= ConfigProvider::get().serverKeepConnectionTime()) {
      table_.erase(it++);
    } else {
      ++it;
    }
  }
}

}