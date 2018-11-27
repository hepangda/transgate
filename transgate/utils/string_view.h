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

#ifndef TRANSGATE_STRING_VIEW_H
#define TRANSGATE_STRING_VIEW_H

#include <cassert>
#include <cstring>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <functional>

#include "../base/copyable.h"
#include "readable_buffer.h"
#include "buffer.h"

namespace tg {

//class StringViewA : public ReadableBuffer, Copyable {
// public:
//  StringView() : ptr_(nullptr), length_(0) {}
//  explicit StringView(const char *ptr) : ptr_(ptr), length_(static_cast<int>(std::strlen(ptr_))) {}
//  explicit StringView(const std::string &str) : ptr_(str.c_str()), length_(static_cast<int>(str.length())) {}
//  StringView(const char *ptr, int length) : ptr_(ptr), length_(length) {}
//  StringView(const std::string &str, int length) : ptr_(str.c_str()), length_(length) {}
//
//  const char *rptr() const override { return ptr_; }
//  int size() const override { return length_; }
//  int readable() const override { return length_; }
//  int read(void *dest, int bytes) {
//
//  }
//  int read(int bytes) final {
//    remove_prefix(bytes);
//    return bytes;
//  }
//
//
//  void doRead(int bytes) override { remove_prefix(bytes); }
//
//  void remove_prefix(int size);
//  void remove_suffix(int size) { length_ -= size; }
//
//  bool operator==(const StringView &rhs) const;
//  bool equalsWithoutCase(const StringView &rhs) const;
// private:
//  const char *ptr_;
//  int length_;
//};

class StringView : public Buffer, public Copyable {
 public:
  StringView() : ptr_(nullptr), length_(0) {}
  explicit StringView(const char *ptr) : ptr_(ptr), length_(static_cast<int>(std::strlen(ptr_))) {}
  explicit StringView(const std::string &str) : ptr_(str.c_str()), length_(static_cast<int>(str.length())) {}
  StringView(const char *ptr, int length) : ptr_(ptr), length_(length) {}
  StringView(const std::string &str, int length) : ptr_(str.c_str()), length_(length) {}

  const char *rptr() const override { return ptr_; }
  int length() const override { return length_; }
  int readable() const override { return length_; }
  int writeable() const override { return 0; }
  void doRead(int bytes) override { remove_prefix(bytes); }

  void remove_prefix(int size);
  void remove_suffix(int size) { length_ -= size; }

  bool operator==(const StringView &rhs) const;
  bool equalsWithoutCase(const StringView &rhs) const;
  bool equalsWithoutCase(const char *rhs) const;

 private:
  const char *ptr_;
  int length_;
};

std::ostream &operator<<(std::ostream &os, const StringView &rhs);

struct StringViewHash {
  size_t operator()(const StringView &string_view) const {
    return std::hash<const char *>()(string_view.rptr()) + std::hash<int>()(string_view.length());
  }
};

}

#endif // TRANSGATE_STRING_VIEW_H
