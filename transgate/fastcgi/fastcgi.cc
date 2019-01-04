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

#include "fcgifwd.h"

#include <cstring>

#include "../net/byteorder.h"

namespace tg {

std::unique_ptr<FcgiHeader> FcgiMakeHeader(FcgiPacketType type, int content_length, int padding_length, int req_id) {
  auto ret = std::make_unique<FcgiHeader>();
  ret->version = kFcgiVersion;
  ret->type = type;
  ret->request_id = HostToNetwork16(req_id);
  ret->content_length = HostToNetwork16(content_length);
  ret->padding_length = static_cast<uint8_t>(padding_length);
  return ret;
}

std::unique_ptr<FcgiBeginRequestBody> FcgiMakeBeginRequestBody(FcgiRole role, FcgiFlags flags) {
  auto ret = std::make_unique<FcgiBeginRequestBody>();
  ret->role = HostToNetwork16(role);
  ret->flags = flags;
  for (auto &i: ret->reserved) {
    i = 0;
  }
  return ret;
}

std::unique_ptr<FcgiEndRequestBody> FcgiMakeEndRequestBody(int app, FcgiProtocolStatus protocol) {
  auto ret = std::make_unique<FcgiEndRequestBody>();
  ret->app_status = HostToNetwork32(app);
  ret->protocol_status = protocol;
  for (auto &i: ret->reserved) {
    i = 0;
  }
  return ret;
}

FcgiParamsPtr FcgiMakeParams(int name_length, int value_length, const char *name, const char *value) {
  auto ret = std::make_unique<char[]>(sizeof(FcgiParamsBody) + name_length + value_length);
  reinterpret_cast<FcgiParamsBody *>(ret.get())->name_length = HostToNetwork32(name_length);
  reinterpret_cast<FcgiParamsBody *>(ret.get())->value_length = HostToNetwork32(value_length);
  std::memcpy(ret.get() + sizeof(FcgiParamsBody), name, static_cast<size_t>(name_length));
  std::memcpy(ret.get() + sizeof(FcgiParamsBody) + name_length, value, static_cast<size_t>(value_length));

  return ret;
}

FcgiParamsPtr FcgiMakeParams(std::string name, std::string value) {
  return FcgiMakeParams(static_cast<int>(name.length()), static_cast<int>(value.length()),
                        name.c_str(), value.c_str());
}

}