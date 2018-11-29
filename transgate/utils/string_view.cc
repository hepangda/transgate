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

#include "string_view.h"

#include <cstring>
#include <iostream>

namespace tg {

void StringView::remove_prefix(int size) {
  assert(size > 0);

  if (size > length_) {
    throw std::invalid_argument("`StringView::remove_prefix` argument `size` is bigger than buffer\'s length");
  }
  ptr_ += size;
  length_ -= size;
}

bool StringView::operator==(const tg::StringView &rhs) const {
  if (length_ != rhs.length_) return false;
  return strncmp(ptr_, rhs.ptr_, static_cast<size_t >(length_)) == 0;
}

bool StringView::equalsWithoutCase(const tg::StringView &rhs) const {
  if (length_ != rhs.length_) return false;
  return strncasecmp(ptr_, rhs.ptr_, static_cast<size_t>(length_)) == 0;
}

bool StringView::equalsWithoutCase(const char *rhs) const {
  if (length_ != strlen(rhs)) return false;
  return strncasecmp(ptr_, rhs, static_cast<size_t>(length_)) == 0;
}

std::ostream &operator<<(std::ostream &os, const StringView &view) {
  os.write(view.readptr(), view.readable());
  return os;
}


}