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
#pragma once
#include <sys/epoll.h>
#include <initializer_list>

#include "../interface/linuxfile.hpp"

namespace tg {

using EpevType = decltype(::epoll_event::events);

enum class EpollEventType : EpevType {
  READABLE = ::EPOLLIN,
  WRITEABLE = ::EPOLLOUT,
  HIGH_PRIORITY = ::EPOLLPRI,
  HANG_UP = ::EPOLLHUP,
  ERROR = ::EPOLLERR,
  EDGE_TRIGGERED = ::EPOLLET,
  LEVEL_TRIGGERED = 0,
  ONESHOT = ::EPOLLONESHOT,           // since Linux 2.6.2
  SOCKET_CLOSED = ::EPOLLRDHUP,       // since Linux 2.6.17
  WAKEUP = ::EPOLLWAKEUP,             // since Linux 3.5
  EPOLLEXCLUSIVE = ::EPOLLEXCLUSIVE,  // since Linux 4.5
};

EpollEventType operator|(EpollEventType lhs, EpollEventType rhs) {
  return EpollEventType(static_cast<int>(lhs) | static_cast<int>(rhs));
}

class EpollEvent : public ILinuxFile {
 private:
  epoll_event _evt;

  inline EpevType getTypeValue(EpollEventType type) {
    return static_cast<EpevType>(type);
  }

 public:
  inline bool checkType(std::initializer_list<EpollEventType> evtlst) {
    auto et = EpollEventType(0);
    for (auto &i : evtlst) {
      et = et | i;
    }
    return _evt.events & getTypeValue(et);
  }

  virtual inline int getFd() override { return _evt.data.fd; }

  inline ::epoll_event *getRawPointer() { return &_evt; }

  template <typename T>
  inline T spawn() {
    return T(getFd());
  }

  EpollEvent() {}

  EpollEvent(ILinuxFile &tr, EpollEventType type) {
    _evt = {getTypeValue(type), {.fd = tr.getFd()}};
  }

  EpollEvent(const epoll_event &ev) { _evt = ev; }
};

}  // namespace tg