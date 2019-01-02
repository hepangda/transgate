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

#ifndef TRANSGATE_GENERIC_BUFFER_H
#define TRANSGATE_GENERIC_BUFFER_H

#include <cstring>
#include <memory>

namespace tg {

class GenericBuffer : public Noncopyable {
 public:
  explicit GenericBuffer(int size): length_(size), addr_(new char[size]) {}
  ~GenericBuffer() { delete[] addr_; }

  int length() const { return length_; }
  void *address() const { return addr_; }
  int mark() const { return mark_; }
  bool get_mark(int mark) const { return (mark_ & mark) == 1; }
  void set_mark(int mark) { mark_ |= mark; }
  void clear_mark() { mark_ = 0; }
  template<typename T>
  T read_cast() { return *reinterpret_cast<T *>(addr_); }
  template <typename T>
  T &write_cast() { return *reinterpret_cast<T *>(addr_); }
 private:
  int mark_ = 0;
  int length_ = 0;
  char *addr_ = nullptr;
};

}

#endif // TRANSGATE_GENERIC_BUFFER_H
