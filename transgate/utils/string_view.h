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

namespace tg {

class StringView : public ReadableBuffer, public Copyable {
 public:
  StringView() = default;
  explicit StringView(const char *ptr) : ptr_(ptr), length_(static_cast<int>(std::strlen(ptr_))) {}
  StringView(const char *ptr, int length) : ptr_(ptr), length_(length) {}

  int readable() const override { return length_; }
  const char *readptr() const final { return ptr_; }
  int size() const final { return length_; }

  int read(void *dest, int bytes) final {
    memcpy(dest, readptr(), static_cast<size_t>(bytes));
    return bytes;
  }
  int read(int bytes) override {
    remove_prefix(bytes);
    return bytes;
  }

  char peek() const final { return *ptr_; }
  char peek(int nums) const final { return ptr_[nums]; }

  void remove_prefix(int size);
  void remove_suffix(int size) { length_ -= size; }

  bool operator==(const StringView &rhs) const;
  bool equalsWithoutCase(const StringView &rhs) const;
  bool equalsWithoutCase(const char *rhs) const;

  std::string toString() const;
 private:
  const char *ptr_ = nullptr;
  int length_ = 0;
};

std::ostream &operator<<(std::ostream &os, const StringView &rhs);

struct StringViewHash {
  size_t operator()(const StringView &string_view) const {
    return std::hash<const char *>()(string_view.readptr()) + std::hash<int>()(string_view.size());
  }
};

}

#endif // TRANSGATE_STRING_VIEW_H
