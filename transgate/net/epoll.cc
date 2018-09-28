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

#include <cassert>

#include "epoll.h"
#include "epoll_event_result.h"

namespace tg {

void Epoll::wait(tg::EpollEventResult &result) {
  int events_triggerd =
      epoll_wait(epoll_fd_, reinterpret_cast<epoll_event *>(result.store_.get()), result.space_size_, WaitForever());

  result.set_length(events_triggerd);
}

void Epoll::waitUntil(tg::EpollEventResult &result, int timeout) {
  assert(timeout > 0);

  int events_triggered =
      epoll_wait(epoll_fd_, reinterpret_cast<epoll_event *>(result.store_.get()), result.space_size_, timeout);

  result.set_length(events_triggered);
}

int Epoll::add(tg::EpollEvent &event) const {
  return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event.event_fd(), event.pointer());
}

int Epoll::add(tg::EpollEvent &&event) const {
  return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event.event_fd(), event.pointer());
}

int Epoll::modify(tg::EpollEvent &event) const {
  return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, event.event_fd(), event.pointer());
}

int Epoll::modify(tg::EpollEvent &&event) const {
  return epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, event.event_fd(), event.pointer());
}

int Epoll::remove(tg::EpollEvent &event) const {
  return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event.event_fd(), event.pointer());
}

int Epoll::remove(const tg::LinuxFile &linux_file) const {
  EpollEvent event(linux_file, EpollEventType(0));
  return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event.event_fd(), event.pointer());
}

}