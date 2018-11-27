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
#include <cstddef>
#include <map>
#include <memory>

#include "../config/config.hpp"
#include "http_types.hpp"

#include "../utils/debug.hpp"
namespace tg {

class HttpError {
 public:
  inline static std::pair<std::shared_ptr<std::byte>, int> spawnErrorStream(
      HttpStatusCode code) {
    static const char *templat =
        "<html><head><title>Transgate/Http Error</title></head><body>"
        "<h1>%s %s</h1><hr />transgate " TRANSGATE_VERSION_STRING
        "</body></html>";

    std::shared_ptr<std::byte> stream{new std::byte[256],
                                      std::default_delete<std::byte[]>()};
    int sz = sprintf((char *)stream.get(), templat,
                     std::to_string(int16_t(code)).c_str(),
                     config::Http::getStatusDesc(int16_t(code)));
    return make_pair(stream, sz);
  }

  inline static HttpStatusCode transcode(HttpRequestStatus st) {
#define CDMAP(code)             \
  case HttpRequestStatus::code: \
    return HttpStatusCode::code
    switch (st) {
      CDMAP(OK);
      CDMAP(NOT_FOUND);
      CDMAP(HTTP_VERSION_NOT_SUPPORTED);
      CDMAP(BAD_REQUEST);
      CDMAP(METHOD_NOT_ALLOWED);
      CDMAP(NOT_IMPLEMENTED);
      CDMAP(FORBIDDEN);
      case HttpRequestStatus::KEEP_ALIVE:
        return HttpStatusCode::OK;
    }
#undef CDMAP
  }
};

}  // namespace tg