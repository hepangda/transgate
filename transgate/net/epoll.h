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

namespace tg {

class EpollEventResult;

class Epoll final : public LinuxFile, public Noncopyable {
 public:
  static constexpr int WaitForever() { return -1; }

  explicit Epoll() : epoll_fd_(epoll_create1(::EPOLL_CLOEXEC)) {}

  int fd() const override { return epoll_fd_; }

  void wait(EpollEventResult &result);
  void waitUntil(EpollEventResult &result, int timeout);

  int add(EpollEvent &event) const;
  int add(EpollEvent &&event) const;

  int remove(EpollEvent &event) const;
  int remove(const LinuxFile &linux_file) const;

  int modify(EpollEvent &event) const;
  int modify(EpollEvent &&event) const;
 private:
  int epoll_fd_;
};

}

#endif //TRANSGATE_EPOLL_H
