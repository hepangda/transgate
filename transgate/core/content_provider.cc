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

#include "content_provider.h"

#include "../utils/file_reader.h"
#include "providers/static_provider.h"

namespace tg {

void ContentProvider::provide() {
  // 检测是否为动态请求
  if (!impl_) {
    impl_ = std::make_unique<StaticProvider>(request_, write_loop_);
  }
  impl_->provide();
}

}