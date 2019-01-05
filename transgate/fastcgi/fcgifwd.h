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

enum FcgiRole : uint16_t {
  kFRResponder = 1,
  kFRAuthorizer = 2,
  kFRFilter = 3,
};

enum FcgiFlags : uint8_t {
  kFFKeepConnection = 1,
};

enum FcgiProtocolStatus : uint8_t {
  kFPSRequestComplete = 0,
  kFPSCantMultiplexConnection = 1,
  kFPSOverloaded = 2,
  kFPSUnknownRole = 3,
};

struct FcgiHeader {
  unsigned char version;
  unsigned char type;
  unsigned char requestIdB1;
  unsigned char requestIdB0;
  unsigned char contentLengthB1;
  unsigned char contentLengthB0;
  unsigned char paddingLength;
  unsigned char reserved;
};

struct FcgiBeginRequestBody {
  unsigned char roleB1;
  unsigned char roleB0;
  unsigned char flags;
  unsigned char reserved[5];
};

struct FcgiEndRequestBody {
  unsigned char appStatusB3;
  unsigned char appStatusB2;
  unsigned char appStatusB1;
  unsigned char appStatusB0;
  unsigned char protocolStatus;
  unsigned char reserved[3];
};

struct FcgiParamsBody {
  unsigned char nameLengthB3;
  unsigned char nameLengthB2;
  unsigned char nameLengthB1;
  unsigned char nameLengthB0;
  unsigned char valueLengthB3;
  unsigned char valueLengthB2;
  unsigned char valueLengthB1;
  unsigned char valueLengthB0;
};

struct FcgiPreread {
  FcgiPacketType type;
  int requestId;
  int contentLength;
  int paddingLength;
};

using FcgiParamsPtr = std::unique_ptr<char[]>;

std::unique_ptr<FcgiHeader> FcgiMakeHeader(FcgiPacketType type, int content_length, int padding_length, int req_id);
std::unique_ptr<FcgiBeginRequestBody> FcgiMakeBeginRequestBody(FcgiRole role, bool keep_alive);
FcgiParamsPtr FcgiMakeParams(int name_length, int value_length, const char *name, const char *value);
FcgiParamsPtr FcgiMakeParams(std::string name, std::string value);

std::unique_ptr<FcgiPreread> FcgiGetHeader(const void *src);
}

#endif // TRANSGATE_FCGI_TYPES_H
