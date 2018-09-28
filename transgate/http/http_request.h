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

#ifndef TRANSGATE_HTTP_REQUEST_H
#define TRANSGATE_HTTP_REQUEST_H

#include <memory>
#include <unordered_map>

#include "../utils/string_view.h"
#include "../base/noncopyable.h"
#include "http_types.h"

namespace tg {

class HttpRequest : public Noncopyable {
 public:
  using FieldContainer = std::unordered_map<StringView, StringView, StringViewHash>;

  void set_method(HttpMethod method) { method_ = method; }
  void set_code(HttpStatusCode code) { code_ = code; }
  void set_uri(const StringView &uri) { uri_ = uri; }
  void set_field(const StringView &key, const StringView &value) { fields_.emplace(key, value); }
  void set_content_starts(const char *content_starts) { content_starts_ = content_starts; }
  void set_content_length(int content_length) { content_length_ = content_length; }
  void set_stream(std::shared_ptr<char> stream) { stream_ = std::move(stream); }
  void set_flag(int flags) { flags_ = flags; }

  HttpMethod method() const { return method_; }
  HttpStatusCode code() const { return code_; }
  StringView uri() const { return uri_; }
  int content_length() const { return content_length_; }
  const char *content() const { return content_starts_; }
  bool flags(HttpFlags flag) const { return (flags_ & flag) != 0; }
  StringView getValue(const StringView &key) const;
  FieldContainer &fields() { return fields_; }

 private:
  HttpMethod method_ = kHMInvalid;
  HttpStatusCode code_ = kHCNotDone;
  StringView uri_;
  FieldContainer fields_;
  int content_length_ = 0;
  const char *content_starts_ = nullptr;
  std::shared_ptr<char> stream_ = nullptr;
  int flags_ = kHFNothing;
};

}
#endif //TRANSGATE_HTTP_REQUEST_H
