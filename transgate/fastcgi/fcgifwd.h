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

#ifndef TRANSGATE_FCGI_TYPES_H
#define TRANSGATE_FCGI_TYPES_H

#include <cstdint>
#include <memory>

namespace tg {

constexpr uint8_t kFcgiVersion = 1;

enum FcgiPacketType {
  kFPTBeginRequest = 1,
  kFPTAbortRequest = 2,
  kFPTEndRequest = 3,
  kFPTParams = 4,
  kFPTStdin = 5,
  kFPTStdout = 6,
  kFPTStderr = 7,
  kFPTData = 8,
  kFPTGetValues = 9,
  kFPTGetValuesResult = 10,
  kFPTUnknown = 11,
};

struct FcgiHeader {
  uint8_t version;
  uint8_t type;
  uint16_t request_id;
  uint16_t content_length;
  uint8_t padding_length;
  uint8_t reserved;
};

enum FcgiRole : uint16_t {
  kFRResponder = 1,
  kFRAuthorizer = 2,
  kFRFilter = 3,
};

enum FcgiFlags : uint8_t {
  kFFKeepConnection = 1,
};

struct FcgiBeginRequestBody {
  uint16_t role;
  uint8_t flags;
  uint8_t reserved[5];
};

enum FcgiProtocolStatus : uint8_t {
  kFPSRequestComplete = 0,
  kFPSCantMultiplexConnection = 1,
  kFPSOverloaded = 2,
  kFPSUnknownRole = 3,
};

struct FcgiEndRequestBody {
  uint32_t app_status;
  uint8_t protocol_status;
  uint8_t reserved[3];
};

struct FcgiParamsBody {
  uint32_t name_length;
  uint32_t value_length;
};

using FcgiParamsPtr = std::unique_ptr<char[]>;


std::unique_ptr<FcgiHeader> FcgiMakeHeader(FcgiPacketType type, int content_length, int padding_length, int req_id);
std::unique_ptr<FcgiBeginRequestBody> FcgiMakeBeginRequestBody(FcgiRole role, FcgiFlags flags);
std::unique_ptr<FcgiEndRequestBody> FcgiMakeEndRequestBody(int app, FcgiProtocolStatus protocol);
FcgiParamsPtr FcgiMakeParams(int name_length, int value_length, const char *name, const char *value);
FcgiParamsPtr FcgiMakeParams(std::string name, std::string value);

}

#endif // TRANSGATE_FCGI_TYPES_H
