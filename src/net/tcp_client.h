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

#ifndef TRANSGATE_TCP_CLIENT_H
#define TRANSGATE_TCP_CLIENT_H

#include "tcp_socket.h"
#include "inet_address.h"

namespace tg {

class TcpClient : public TcpSocket /* Noncopyable, LinuxFile */ {
 public:
  explicit TcpClient(InetAddress &&addr): addr_(addr), TcpSocket() {}
  TcpClient(int fd, const InetAddress &addr): TcpSocket(fd), addr_(addr) {}
  TcpClient(int fd, InetAddress &&addr): TcpSocket(fd), addr_(addr) {}

  InetAddress address() const { return addr_; }
  void connect();
 private:
  InetAddress addr_;
  int bad_ = 0;
};

}

#endif // TRANSGATE_TCP_CLIENT_H
