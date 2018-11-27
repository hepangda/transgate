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

#ifndef TRANSGATE_WRITE_LOOP_H
#define TRANSGATE_WRITE_LOOP_H

#include <deque>
#include <memory>
#include "../utils/char_buffer.h"

namespace tg {

enum WriteEventType : int {
  kWETSend = 0,
  kWETSendFile = 1,
};

class WriteLoop {
 public:
  WriteLoop(int fd, int buffer_size) : fd_(fd), buffer_(std::make_unique<CharBuffer>(buffer_size)) {}
  struct Task {
    Task() = default;
    Task(WriteEventType type, int length, int fd) : type(type), length(length), fd(fd) {}
    WriteEventType type;
    int length;
    int fd;
  };

  bool doOnce();
  void doAll() { while (doOnce()); }

  void insert(WriteEventType type, int length, int offset) {
    q_.emplace_front(type, length, offset);
  }
  void append(WriteEventType type, int length, int offset) {
    q_.emplace_back(type, length, offset);
  }

  int write(int bytes) { return buffer_->write(bytes); }
  int write(const char *src, int bytes) { return buffer_->write(src, bytes); }
  int write(const char *src) { return buffer_->write(src); }
  int write(const std::string &str, int bytes = -1) { return buffer_->write(str, bytes); }

 private:
  int fd_;
  std::unique_ptr<CharBuffer> buffer_;
  std::deque<Task> q_;

  bool actSend(int length);
  bool actSendfile(int length, int fd);
};

}

#endif // TRANSGATE_WRITE_LOOP_H
