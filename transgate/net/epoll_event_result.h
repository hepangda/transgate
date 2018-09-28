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


#ifndef TRANSGATE_EPOLL_EVENT_RESULT_H
#define TRANSGATE_EPOLL_EVENT_RESULT_H

#include <memory>

#include "../base/noncopyable.h"
#include "epoll_event.h"

namespace tg {

class Epoll;

class EpollEventResult final : public Noncopyable {
  friend class Epoll;
 public:
  explicit EpollEventResult(int size) : store_(new EpollEvent[size]), length_(0), space_size_(size) {}

  int size() const { return length_; }
  const EpollEvent &at(int i) { return store_.get()[i]; }
  const EpollEvent operator[] (int i) { return store_.get()[i]; }
 private:
  std::unique_ptr<EpollEvent> store_;
  int length_;
  int space_size_;

  void set_length(int length) { length_ = length; }
};

}

#endif //TRANSGATE_EPOLL_EVENT_RESULT_H
