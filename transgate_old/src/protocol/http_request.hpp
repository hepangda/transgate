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
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "http_types.hpp"

namespace tg {

class HttpRequest {
 public:
  // private:
  using FieldContainer = std::unordered_map<std::string, std::string>;

  HttpMethod method;
  std::string request_uri;
  FieldContainer fields;
  size_t entity_size;
  std::shared_ptr<std::byte> entity;
  HttpRequestStatus status = HttpRequestStatus::OK;
  int finish = -1;
  std::string_view reverse_key;

 public:
  inline std::string *getFieldValue(std::string &&key) {
    if (fields.find(key) != fields.end()) {
      return &fields[key];
    }

    return nullptr;
  }

  inline void setEntity(std::byte *ent, int size) {
    this->entity.reset(ent, std::default_delete<std::byte[]>());
    this->entity_size = size;
  }

  inline std::byte *getEntity() { return this->entity.get(); }

  inline std::size_t entitySize() { return this->entity_size; }

  inline void setReverseKey(std::string_view sv) { this->reverse_key = sv; }

  inline std::string_view getReverseKey() { return this->reverse_key; }

  inline void setMethod(HttpMethod met) { this->method = met; }

  inline const HttpMethod &getMethod() const { return this->method; }

  inline void setUri(std::string &&uri) { this->request_uri = uri; }

  inline void setUri(std::string_view uri, std::string_view::size_type sz) {
    this->request_uri = std::string(uri.data(), sz);
  }

  inline const std::string &getUri() const { return this->request_uri; }

  inline void pushField(std::string &&key, std::string &&value) {
    this->fields.emplace(key, value);
  }

  inline HttpRequestStatus getStatus() { return this->status; }

  inline void setStatus(HttpRequestStatus status) { this->status = status; }
};

}  // namespace tg