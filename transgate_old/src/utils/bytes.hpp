/****************************************************************************
 * Copyright 2018 hepangda                                                  *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *     http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/
#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>

#include "../utils/debug.hpp"

namespace tg {

using Byte = unsigned char;

class ByteBuffer {
 private:
  std::shared_ptr<std::byte> _datap;
  size_t _size = 0;

 public:
  ByteBuffer(size_t size = 10) {
    _datap = {new std::byte[size], std::default_delete<std::byte[]>()};
    _size = size;
  }

  ByteBuffer(const ByteBuffer &buf) {
    _datap = buf._datap;
    _size = buf._size;
  }

  inline void redirect(std::byte *buf, std::size_t size) {
    _datap.reset(buf);
    _size = size;
  }

  template <typename T = void>
  inline T *pointer(std::size_t from = 0) {
    return reinterpret_cast<T *>(_datap.get() + from);
  }

  inline std::size_t size() { return _size; }

  inline void clean() {
    for (int i = 0; i < _size; i++) {
      *(_datap.get() + i) = std::byte(0);
    }
  }
};

}  // namespace tg
