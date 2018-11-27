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

#include <unistd.h>
#include <vector>

#include "../config/config.hpp"
#include "epoll_event.hpp"

namespace tg {

class Epoll : public ILinuxFile {
 private:
  int epfd = -1;

  static constexpr int TGEPOLL_WAIT_ALWAYS = -1;

 public:
  Epoll() { this->epfd = ::epoll_create1(::EPOLL_CLOEXEC); }

  Epoll(const Epoll &) = delete;

  virtual ~Epoll() { ::close(epfd); }

  inline std::vector<EpollEvent> wait() {
    return waitTimeout(TGEPOLL_WAIT_ALWAYS);
  }

  // Note that if timeout equals to TGEPOLL_WAIT_ALWAYS(-1),
  //      it does the same work as wait()
  // if timeout equals to 0, wait returns immediately
  std::vector<EpollEvent> waitTimeout(int timeout) {
    ::epoll_event evar[config::TGEPOLL_EVN];
    int evts = ::epoll_wait(epfd, evar, config::TGEPOLL_EVN, timeout);

    std::vector<EpollEvent> ret(evar, evar + evts);
    return std::move(ret);
  }

  int add(EpollEvent evt) {
    return ::epoll_ctl(epfd, EPOLL_CTL_ADD, evt.getFd(), evt.getRawPointer());
  }

  int modify(EpollEvent evt) {
    return ::epoll_ctl(epfd, EPOLL_CTL_MOD, evt.getFd(), evt.getRawPointer());
  }

  int remove(EpollEvent evt) {
    // Tips: Before Linux 2.6.9, the last parameter ingored, but required a
    // non-null pointer
    return ::epoll_ctl(epfd, EPOLL_CTL_DEL, evt.getFd(), nullptr);
  }

  int remove(int fd) {
    // See overloads of remove(EpollEvent)
    return ::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
  }

  virtual inline int getFd() override { return epfd; }
};

}  // namespace tg