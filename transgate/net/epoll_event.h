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

#ifndef TRANSGATE_EPOLL_EVENT_H
#define TRANSGATE_EPOLL_EVENT_H

#include <sys/epoll.h>
#include <initializer_list>

#include "../base/linuxfile.h"
#include "../base/copyable.h"

namespace tg {

enum EpollEventType {
  kEPReadable = ::EPOLLIN,
  kEPWriteable = ::EPOLLOUT,
  kEPHighPriority = ::EPOLLPRI,
  kEPHangUp = ::EPOLLHUP,
  kEPEdgeTriggered = ::EPOLLET,
  kEPLevelTriggered = 0,
  kEPOneShot = ::EPOLLONESHOT,
  kEPSocketClosed = ::EPOLLRDHUP,
  kEPWakeUp = ::EPOLLWAKEUP,
  kEPExclusive = ::EPOLLEXCLUSIVE,
  KEPError = ::EPOLLERR
};

constexpr EpollEventType ETEOBasic() { return EpollEventType(kEPEdgeTriggered | kEPOneShot | kEPSocketClosed); }
constexpr EpollEventType ETEOReadable() { return EpollEventType(ETEOBasic() | kEPReadable); }
constexpr EpollEventType ETEOWriteable() { return EpollEventType(ETEOBasic() | kEPWriteable); }
constexpr EpollEventType ETEOAllof() { return EpollEventType(ETEOReadable() | ETEOWriteable()); }

class EpollEvent final : public Copyable {
 public:
  explicit EpollEvent(): event_() {}
  EpollEvent(const LinuxFile &linux_file, EpollEventType type): event_() {
    event_ = epoll_event{type, {.fd = linux_file.fd()}};
  }

  bool check(EpollEventType evt) const { return (event_.events & evt) != 0; }
  bool check(std::initializer_list<EpollEventType> evt) const {
    for (const auto &i: evt) {
      if ((event_.events & i) != 0) {
        return true;
      }
    }
    return false;
  }

  bool happenedOn(const LinuxFile &linux_file) const { return event_.data.fd == linux_file.fd(); }
  int event_fd() const { return event_.data.fd; }
  epoll_event *pointer() { return &event_; }

 private:
  epoll_event event_;
};

}

#endif // TRANSGATE_EPOLL_EVENT_H
