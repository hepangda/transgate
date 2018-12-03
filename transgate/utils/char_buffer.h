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

#include <memory>

#include "../base/noncopyable.h"
#include "readable_buffer.h"

namespace tg {

class CharBuffer : public ReadableBuffer, Noncopyable {
 public:
  explicit CharBuffer(int size) : size_(size), store_(std::make_unique<char[]>(static_cast<size_t>(size))) {}

  int readable() const final { return write_pos_ - read_pos_; }
  int writeable() const { return size_ - write_pos_; }
  int size() const final { return size_; }

  const char *readptr() const final { return store_.get() + read_pos_; }
  void *writeptr() const { return store_.get() + write_pos_; }

  int read(void *dest, int bytes) final;
  int read(int bytes) final;

  char peek() const final { return peek(0); }
  char peek(int nums) const final;

  int write(int bytes);
  int write(const char *src, int bytes);
  int write(const char *src);
  int write(const std::string &str) { return write(str, static_cast<int>(str.length())); }
  int write(const std::string &str, int bytes = -1);
  int swrite(const char *format, va_list va);

  const char operator[](int sub) const { return store_[sub + read_pos_]; }
 private:
  std::unique_ptr<char[]> store_;
  int size_;
  int write_pos_ = 0;
  int read_pos_ = 0;

  bool clean(int bytes);
};

}

#endif // TRANSGATE_CHAR_BUFFER_H
