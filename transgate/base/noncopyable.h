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

#ifndef TRANSGATE_NONCOPYABLE_H
#define TRANSGATE_NONCOPYABLE_H

namespace tg {

class Noncopyable {
 public:
  // delete copy constructor and copy assign operator to avoid copy
  Noncopyable(const Noncopyable &) = delete;
  Noncopyable &operator=(const Noncopyable &) = delete;
 protected:
  // ｄeclared `protected` to avoid get instance of `Noncopyable` itself
  constexpr Noncopyable() = default;
  ~Noncopyable() = default;
};

}

#endif //TRANSGATE_NONCOPYABLE_H
