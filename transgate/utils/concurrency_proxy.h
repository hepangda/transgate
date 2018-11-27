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

#ifndef TRANSGATE_CONCURRENCY_PROXY_H
#define TRANSGATE_CONCURRENCY_PROXY_H

#include <thread>
#include <vector>

class ConcurrencyProxy {
 public:
  template <typename Callable, typename ...Args>
  explicit ConcurrencyProxy(Callable func, Args ...args) {
    thcont_.reserve(std::thread::hardware_concurrency());

//    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
      thcont_.emplace_back(std::forward<Callable>(func), std::forward<Args>(args)...);
//    }
  }

  int wait() {
    for (auto &i: thcont_) {
      i.join();
    }

    return 0;
  }
 private:
  std::vector<std::thread> thcont_;
};

#endif // TRANSGATE_CONCURRENCY_PROXY_H
