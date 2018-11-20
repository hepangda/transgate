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

#ifndef TRANSGATE_EPOLL_H
#define TRANSGATE_EPOLL_H

#include <sys/epoll.h>

#include "epoll_event.h"
#include "../base/linuxfile.h"
#include "../base/noncopyable.h"
#include "epoll_event_result.h"

namespace tg {

class EpollEventResult;

class Epoll final : public LinuxFile, public Noncopyable {
 public:
  static constexpr int WaitForever() { return -1; }

  explicit Epoll() : epoll_fd_(epoll_create1(::EPOLL_CLOEXEC)) {}

  int fd() const override { return epoll_fd_; }

  void wait(EpollEventResult &result) { waitUntil(result, WaitForever()); }
  void waitUntil(EpollEventResult &result, int timeout) {
    int events_triggered =
        epoll_wait(epoll_fd_, reinterpret_cast<epoll_event *>(result.store_.get()), result.space_size_, timeout);

    result.set_length(events_triggered);
  }

  int add(EpollEvent &event) const {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event.event_fd(), event.pointer());
  }
  int add(EpollEvent &&event) const {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event.event_fd(), event.pointer());
  }
  int add(const LinuxFile &linux_file, EpollEventType event_type) const {
    return add({linux_file, event_type});
  }

  int remove(EpollEvent &event) const {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event.event_fd(), event.pointer());
  }
  int remove(const EpollEvent &event) const {
    EpollEvent evt { event };
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, evt.event_fd(), evt.pointer());
  }
  int remove(const LinuxFile &linux_file) const {
    EpollEvent event { linux_file, EpollEventType(0) };
    return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event.event_fd(), event.pointer());
  }

  int modify(EpollEvent &event) const {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, event.event_fd(), event.pointer());
  }
  int modify(EpollEvent &&event) const {
    return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, event.event_fd(), event.pointer());
  }
  int modify(const LinuxFile &linux_file, EpollEventType event_type) const {
    return modify({linux_file, event_type});
  }
 private:
  int epoll_fd_;
};

}

#endif // TRANSGATE_EPOLL_H
