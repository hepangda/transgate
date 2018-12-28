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

#include "static_provider.h"

#include <cctype>
#include <iostream>

#include "../../utils/file_reader.h"
#include "../../http/http_types.h"

namespace tg {

namespace detail {

std::string getUri(const HttpRequest &request) {
  if (request.uri().size() == 1) {
    return ".";
  } else if (request.uri().size() > 0 && *request.uri().readptr() == '/') {
    return std::move(std::string(request.uri().readptr() + 1, static_cast<unsigned int>(request.uri().size() - 1)));
  }
  return request.uri().toString();
}

}

void StaticProvider::provide() {
  std::shared_ptr<FileReader> file = nullptr;
  bool cond = staticProvide(file);
  if (cond) {
    request_->set_code(kHCOk);
    int ret = regularProvide(true, file->size());
    ret += writeCRLF();
    write_loop_->appendSend(ret);
    write_loop_->appendSendfile(file);
    ret = writeCRLF();
    write_loop_->appendSend(ret);
  } else {
    provideError();
  }
}

bool StaticProvider::staticProvide(std::shared_ptr<FileReader> &file) {
  if (!request_->good())
    return false;
  if (host_config_->isForbidden(request_->uri().toString().c_str())) {
    request_->set_code(kHCForbidden);
    return false;
  }

  file = std::make_shared<FileReader>(*host_config_->wwwroot(), detail::getUri(*request_));

  if (file->good() && file->isDirectory()) {
    file = host_config_->defaultFile(detail::getUri(*request_).c_str());
  }

  if (!file || !file->good()) {
    request_->set_code(kHCNotFound);
    return false;
  }

  return true;
}

}