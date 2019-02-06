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

#ifndef TRANSGATE_INET_ADDRESS_H
#define TRANSGATE_INET_ADDRESS_H

#include <arpa/inet.h>

#include "../base/copyable.h"

namespace tg {

class InetAddress final: public Copyable {
 public:
  explicit InetAddress(const char *address, int port);
  explicit InetAddress(int port, bool loop_back_only = false);
  explicit InetAddress(const struct sockaddr_in &addr4) : addr_(addr4) {}

  struct sockaddr *pointer() { return static_cast<struct sockaddr *>(static_cast<void *>(&addr_)); }
  constexpr socklen_t length() const { return sizeof(addr_); }
 private:
  struct sockaddr_in addr_;
};

}

#endif // TRANSGATE_INET_ADDRESS_H
