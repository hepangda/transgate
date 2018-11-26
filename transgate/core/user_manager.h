//
// Created by pangda on 11/26/18.
//

#ifndef TRANSGATE_USER_MANAGER_H
#define TRANSGATE_USER_MANAGER_H

#include <memory>
#include <unordered_map>
#include "../http/http_user.h"
#include "../net/epoll.h"

namespace tg {

class UserManager {
 public:
  bool contains(int id) { return table_.count(id) > 0; }

  int delegate(std::unique_ptr<HttpUser> &user) {
    int ret = user->fd();
    table_.emplace(ret, user.release());
    return ret;
  };

  int delegate(std::unique_ptr<HttpUser> &&user) {
    int ret = user->fd();
    table_.emplace(ret, user.release());
    return ret;
  }

  void doReadable(int id) {
    if (!contains(id)) {
      throw std::invalid_argument("`UserManager::doReadable` no such user.");
    }
    table_[id]->onRead();
  }


  void addEventsOfReadable(Epoll &epoll, int id) {
    if (!contains(id)) {
      throw std::invalid_argument("`UserManager::addEventsOfReadable` no such user.");
    }
    epoll.add(*table_[id], ETEOReadable());
  }

  void release(int id) {
    if (!contains(id)) {
      throw std::invalid_argument("`UserManager::release` released the incorrect user.");
    }
    table_.erase(id);
  }
 private:
  std::unordered_map<int, std::unique_ptr<HttpUser>> table_;
};

}

#endif // TRANSGATE_USER_MANAGER_H
