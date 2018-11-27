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

#include "inet_address.h"

#include <strings.h>
#include <cassert>
#include <stdexcept>

namespace tg {

InetAddress::InetAddress(const char *address, int port) {
  assert(port > 0);

  bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(static_cast<uint16_t >(port));
  if (inet_pton(AF_INET, address, &addr_) <= 0) {
    throw std::runtime_error("`InetAddress::InetAddress` Cannot convert IPv4 address/port to struct");
  }
}

InetAddress::InetAddress(int port, bool loop_back_only) {
  assert(port > 0);

  bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(static_cast<uint16_t>(port));
  addr_.sin_addr.s_addr = loop_back_only ? INADDR_LOOPBACK : INADDR_ANY;
}

}