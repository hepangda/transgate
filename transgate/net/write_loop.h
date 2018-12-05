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
#include <functional>
#include "../utils/char_buffer.h"

namespace tg {

class FileReader;

class WriteLoop {
 public:
  WriteLoop(int fd, int buffer_size) : fd_(fd), buffer_(std::make_unique<CharBuffer>(buffer_size)) {}
  using Task = std::function<bool()>;

  bool empty() const { return q_.empty(); }
  bool doOnce();
  void doAll() { while (doOnce()) {}}

  void appendSend(int length) { q_.emplace_back([this, length] { return actSend(length); }); }
  void appendSend() { appendSend(buffer_->readable()); }
  void appendSendfile(const std::shared_ptr<FileReader> &file) { q_.emplace_back([this, file] { return actSendfile(file); }); }
  template <typename Callable>
  void appendChore(Callable f) { q_.emplace_back(f); }

  int write(int bytes) { return buffer_->write(bytes); }
  int write(const char *src, int bytes) { return buffer_->write(src, bytes); }
  int write(const char *src) { return buffer_->write(src); }
  int write(const std::string &str, int bytes = -1) { return buffer_->write(str, bytes); }
  int swrite(const char *format, ...);
 private:
  int fd_;
  std::unique_ptr<CharBuffer> buffer_;
  std::deque<Task> q_;

  bool actSend(int length);
  bool actSendfile(std::shared_ptr<FileReader> file);

  void insertSend(int length) { q_.emplace_front([this, length] { return actSend(length); }); }
  void insertSendfile(const std::shared_ptr<FileReader> &file) { q_.emplace_back([this, file] { return actSendfile(file); }); }
};

}

#endif // TRANSGATE_WRITE_LOOP_H
