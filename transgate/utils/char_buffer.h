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

#ifndef TRANSGATE_CHAR_BUFFER_H
#define TRANSGATE_CHAR_BUFFER_H
#include <iostream>
#include <memory>
#include <cstring>

#include "../base/noncopyable.h"
#include "readable_buffer.h"

namespace tg {

class CharBuffer : public ReadableBuffer, Noncopyable {
 public:
  explicit CharBuffer(int size) : size_(size), store_(std::make_unique<char[]>(size)) {}

  int readable() const final { return write_pos_ - read_pos_; }
  int writeable() const { return size_ - write_pos_; }
  int size() const final { return size_; }

  const char *readptr() const final { return store_.get() + read_pos_; }
  void *writeptr() const { return store_.get() + write_pos_; }

  int read(void *dest, int bytes) final {
    memcpy(dest, store_.get() + read_pos_, static_cast<size_t>(bytes));
    read(bytes);
    return bytes;
  }

  int read(int bytes) final {
    if (bytes > readable())
      throw std::invalid_argument("`CharBuffer::read` argument `bytes` is over than readable.");
    read_pos_ += bytes;
    return bytes;
  }

  char peek() const final {
    return peek(0);
  }

  char peek(int nums) const final {
    if (nums + 1 >= readable()) {
      throw std::invalid_argument("`CharBuffer::peek` arguemnt `nums` is over than readable.");
    }
    return store_[read_pos_ + nums];
  }

  int write(int bytes) {
    if (bytes > writeable() && !clean(bytes))
      throw std::invalid_argument("`CharBuffer::write` argument `bytes` is over than writeable.");
    write_pos_ += bytes;
    return bytes;
  }

  int write(const char *src, int bytes) {
    if (bytes >= writeable() && !clean(bytes)) {
      throw std::invalid_argument("`CharBuffer::write` arguemnt `bytes` is over than writeable.");
    }

    memcpy(store_.get() + write_pos_, src, static_cast<size_t>(bytes));
    write_pos_ += bytes;
    return bytes;
  }

  int write(const char *src) {
    return write(src, static_cast<int>(std::strlen(src)));
  }

  int write(const std::string &str, int bytes = -1) {
    if (bytes == -1)
      bytes = static_cast<int>(str.length());
    else if (bytes > str.length())
      throw std::invalid_argument("`CharBuffer::write` argument `bytes` is over than string\'s length.");
    return write(str.c_str(), bytes);
  }

  const char operator[](int sub) const { return store_[sub + read_pos_]; }
 private:
  std::unique_ptr<char[]> store_;
  int size_;
  int write_pos_ = 0;
  int read_pos_ = 0;

  bool clean(int bytes) {
    int interval = write_pos_ - read_pos_;
    if (bytes > size_ - 128) return false;
    // fix magic number

    memmove(store_.get(), readptr(), static_cast<size_t>(interval));
    write_pos_ = read_pos_ = 0;
    return true;
  }
};

}

#endif // TRANSGATE_CHAR_BUFFER_H
