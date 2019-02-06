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

#include "char_buffer.h"

#include <cstring>
#include <cstdarg>

namespace tg {

int CharBuffer::read(void *dest, int bytes) {
  std::memcpy(dest, store_.get() + read_pos_, static_cast<size_t>(bytes));
  read(bytes);
  return bytes;
}

int CharBuffer::read(int bytes) {
  if (bytes > readable())
    throw std::invalid_argument("`CharBuffer::read` argument `bytes` is over than readable.");
  read_pos_ += bytes;
  return bytes;
}

char CharBuffer::peek(int nums) const {
  if (nums + 1 > readable()) {
    throw std::invalid_argument("`CharBuffer::peek` arguemnt `nums` is over than readable.");
  }
  return store_[read_pos_ + nums];
}

int CharBuffer::write(int bytes) {
  if (bytes > writeable() && !clean(bytes))
    throw std::invalid_argument("`CharBuffer::write` argument `bytes` is over than writeable.");
  write_pos_ += bytes;
  return bytes;
}

int CharBuffer::write(const char *src, int bytes) {
  if (bytes > writeable() && !clean(bytes)) {
    throw std::invalid_argument("`CharBuffer::write` arguemnt `bytes` is over than writeable.");
  }

  memcpy(store_.get() + write_pos_, src, static_cast<size_t>(bytes));
  write_pos_ += bytes;
  return bytes;
}

int CharBuffer::write(const char *src) {
  return write(src, static_cast<int>(std::strlen(src)));
}

int CharBuffer::write(const std::string &str, int bytes) {
  if (bytes > str.length())
    throw std::invalid_argument("`CharBuffer::write` argument `bytes` is over than string\'s length.");
  return write(str.c_str(), bytes);
}

int CharBuffer::swrite(const char *format, va_list va) {
  return vsnprintf(store_.get() + write_pos_, static_cast<size_t>(writeable()), format, va);
}

bool CharBuffer::clean(int bytes) {
  int interval = write_pos_ - read_pos_;
  if (bytes > size_ - 128) return false;
  // todo:fix magic number

  memmove(store_.get(), readptr(), static_cast<size_t>(interval));
  read_pos_ = 0;
  write_pos_ = interval;
  return true;
}

void CharBuffer::move() {
  int interval = write_pos_ - read_pos_;
  memmove(store_.get(), readptr(), static_cast<size_t>(interval));
  read_pos_ = 0;
  write_pos_ = interval;
}

}