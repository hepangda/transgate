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
#include "config_provider.h"
#include "providers/fastcgi_provider.h"
#include "providers/null_provider.h"
#include "core_marks.h"

#include <iostream>
namespace tg {

namespace detail {

bool isFilename(char x) {
  return !(x == '?' || x == '\\' || x == '/' || x == '*' ||
      x == '\"' || x == '\'' || x == '<' || x == '>' || x == '|');
}

std::string fileExtends(const StringView &uri) {
  const char *ext_start = uri.readptr() + uri.size(),
      *ext_end = ext_start;

  for (; ext_start != uri.readptr(); ext_start--) {
    if (*ext_start == '.') {
      break;
    } else if (!isFilename(*ext_start)) {
      ext_end = ext_start;
    }
  }

  return ext_start == uri.readptr() ?
         "" : std::string(ext_start, static_cast<size_t>(std::distance(ext_start, ext_end)));
}
}

void ContentProvider::provide(std::shared_ptr<GenericBuffer> interaction_buffer_) {
  static const StringView host_string{"host"};

  auto value = request_->getValue(host_string).toString();
  auto host = ConfigProvider::get().adaptHost(value.c_str());
  impl_ = nullptr;

  if (host && host->isEnabledFastcgi()) {
    auto fcgi_config = host->adaptFcgi(detail::fileExtends(request_->uri()).c_str());
    if (fcgi_config) {
      interaction_buffer_->write_cast<FastcgiProvider>() = {request_, write_loop_, host, fcgi_config};
      interaction_buffer_->set_mark(kCMFcgi);
      impl_ = std::make_unique<NullProvider>();
    }
  } else {
    request_->set_bad();
    request_->set_code(kHCNotFound);
  }

  if (!impl_) {
    impl_ = std::make_unique<StaticProvider>(request_, write_loop_, host);
  }
  impl_->provide();
}

}