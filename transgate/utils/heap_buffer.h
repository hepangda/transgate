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

#ifndef TRANSGATE_HEAP_BUFFER_H
#define TRANSGATE_HEAP_BUFFER_H

#include <memory>

#include "../base/copyable.h"
#include "buffer.h"

namespace tg {

class HeapBuffer : public Buffer, public Copyable {
 public:
  explicit HeapBuffer(int length): ptr_length_(length), ptr_(std::make_unique<char>(length)) {}

  HeapBuffer(const HeapBuffer &rhs);
  HeapBuffer &operator =(const HeapBuffer &rhs);

  inline void *wptr() { return ptr_.get() + readed_bytes_; }
  inline const char *rptr() const override { return ptr_.get(); }
  inline int length() const override {  return ptr_length_; }
  inline int readable() const override { return writed_bytes_ - readed_bytes_; }
  inline int writeable() const override { return ptr_length_ - writed_bytes_; }

  void doRead(int bytes) override;
  void doWrite(int bytes);

 private:
  std::unique_ptr<char> ptr_;
  int ptr_length_;
  int readed_bytes_ = 0;
  int writed_bytes_ = 0;
};

}

#endif //TRANSGATE_HEAP_BUFFER_H
