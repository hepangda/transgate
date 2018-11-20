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

#include "heap_buffer.h"

#include <cassert>

namespace tg {

HeapBuffer::HeapBuffer(const tg::HeapBuffer &rhs) {
  ptr_length_ = rhs.ptr_length_;
  ptr_ = std::make_unique<char>(rhs.ptr_length_);
  readed_bytes_ = rhs.readed_bytes_;
  writed_bytes_ = rhs.writed_bytes_;
  std::copy(rhs.ptr_.get(), rhs.ptr_.get() + rhs.ptr_length_, ptr_.get());
}

HeapBuffer &HeapBuffer::operator=(const HeapBuffer &rhs) {
  assert(&rhs == this);

  ptr_length_ = rhs.ptr_length_;
  ptr_ = std::make_unique<char>(rhs.ptr_length_);
  readed_bytes_ = rhs.readed_bytes_;
  writed_bytes_ = rhs.writed_bytes_;
  std::copy(rhs.ptr_.get(), rhs.ptr_.get() + rhs.ptr_length_, ptr_.get());

  return *this;
}

void HeapBuffer::doRead(int bytes) {
  if (bytes > readable()) {
    throw std::invalid_argument("`HeapBuffer::doRead` argument `Readbytes` is over than readable.");
  }
  readed_bytes_ += bytes;

  // TODO: if the diff of readed_bytes and writed_bytes are less than a specific value, move and reset
  int diff = writed_bytes_ - readed_bytes_;
  if (diff <= 128 && readed_bytes_ >= length() / 5) {
    for (int i = 0; i < diff; i++) {

    }
  } else if (diff == 0) {
    readed_bytes_ = writed_bytes_ = 0;
  }
}

void HeapBuffer::doWrite(int bytes) {
  if (bytes > writeable()) {
    throw std::invalid_argument("`HeapBuffer::doWrite` argument `Writebytes` is over than writeable.");
  }

  writed_bytes_ += bytes;
}

}
