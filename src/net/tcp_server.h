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

#ifndef TRANSGATE_TCP_SERVER_H
#define TRANSGATE_TCP_SERVER_H
#include <functional>
#include <memory>

#include "tcp_socket.h"
#include "inet_address.h"

namespace tg {

class TcpClient;

class TcpServer : public TcpSocket /* Noncopyable, LinuxFile */ {
 public:
//  explicit TcpServer(int fd): TcpSocket(fd) {}
  explicit TcpServer(const InetAddress &addr): server_addr_(std::make_unique<InetAddress>(addr)) {}

  std::unique_ptr<TcpSocket> accept();
  void acceptAll(std::function<void(int)> cb);
  std::unique_ptr<TcpClient> acceptWithAddress();
  int bind();
  int listen();

  static struct {} ReusePort;
  static struct {} ReuseAddr;
  int setVar(decltype(ReuseAddr));
  int setVar(decltype(ReusePort));

  void bindAndListen();
 private:
  std::unique_ptr<InetAddress> server_addr_ = nullptr;
};

}

#endif // TRANSGATE_TCP_SERVER_H