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

namespace tg {

template<typename T>
unsigned char cast_uc(T n) {
  return static_cast<unsigned char>(n);
}

std::unique_ptr<FcgiHeader> FcgiMakeHeader(FcgiPacketType type, int content_length, int padding_length, int req_id) {
  auto ret = std::make_unique<FcgiHeader>();
  ret->version = cast_uc(kFcgiVersion);
  ret->type = cast_uc(type);
  ret->requestIdB1 = cast_uc((req_id >> 8) & 0xff);
  ret->requestIdB0 = cast_uc(req_id & 0xff);
  ret->contentLengthB1 = cast_uc((content_length >> 8) & 0xff);
  ret->contentLengthB0 = cast_uc(content_length & 0xff);
  ret->paddingLength = cast_uc(padding_length);
  ret->reserved = 0;
  return ret;
}

std::unique_ptr<FcgiBeginRequestBody> FcgiMakeBeginRequestBody(FcgiRole role, bool keep_alive) {
  auto ret = std::make_unique<FcgiBeginRequestBody>();

  ret->roleB1 = cast_uc((role >> 8) & 0xff);
  ret->roleB0 = cast_uc(role & 0xff);
  ret->flags = keep_alive ? kFFKeepConnection : 0;
  for (unsigned char &i : ret->reserved) {
    i = 0;
  }
  return ret;
}

FcgiParamsPtr FcgiMakeParams(int name_length, int value_length, const char *name, const char *value) {
  auto ret = std::make_unique<char[]>(sizeof(FcgiParamsBody) + name_length + value_length);
  auto base = reinterpret_cast<FcgiParamsBody *>(ret.get());

  base->nameLengthB3 = cast_uc((name_length >> 24) | 0x80);
  base->nameLengthB2 = cast_uc(name_length >> 16);
  base->nameLengthB1 = cast_uc(name_length >> 8);
  base->nameLengthB0 = cast_uc(name_length);
  base->valueLengthB3 = cast_uc((value_length >> 24) | 0x80);
  base->valueLengthB2 = cast_uc(value_length >> 16);
  base->valueLengthB1 = cast_uc(value_length >> 8);
  base->valueLengthB0 = cast_uc(value_length);

  std::memcpy(ret.get() + sizeof(FcgiParamsBody), name, static_cast<size_t>(name_length));
  std::memcpy(ret.get() + sizeof(FcgiParamsBody) + name_length, value, static_cast<size_t>(value_length));
  return ret;
}

FcgiParamsPtr FcgiMakeParams(std::string name, std::string value) {
  return FcgiMakeParams(static_cast<int>(name.length()), static_cast<int>(value.length()),
                        name.c_str(), value.c_str());
}

std::unique_ptr<FcgiPreread> FcgiGetHeader(const void *src) {
  FcgiHeader head = *(static_cast<const FcgiHeader *>(src));
  auto ret = std::make_unique<FcgiPreread>();
  ret->type = FcgiPacketType(head.type);
  ret->requestId = (head.requestIdB1 << 8) + head.requestIdB0;
  ret->contentLength = (head.contentLengthB1 << 8) + head.contentLengthB0;
  ret->paddingLength = head.paddingLength;

  return ret;
}

}