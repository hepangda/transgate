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

#include "tcp_server.h"

#include <cassert>

#include "tcp_client.h"

namespace tg {

TcpSocket TcpServer::accept() {
  return TcpSocket(::accept4(fd(), nullptr, nullptr, SOCK_NONBLOCK));
}

TcpClient TcpServer::acceptWithAddress() {
  struct sockaddr_in addr = {};
  socklen_t socklen = sizeof(addr);

  int client_fd = ::accept4(fd(), reinterpret_cast<struct sockaddr *>(&addr), &socklen, SOCK_NONBLOCK);
  return { client_fd, InetAddress(addr) };
}

int TcpServer::bind() {
  return ::bind(fd(), server_addr_->pointer(), server_addr_->length());
}

int TcpServer::listen() {
  return ::listen(fd(), SOMAXCONN);
}

void TcpServer::bindAndListen() {
  assert(server_addr_ != nullptr);

  if (bind() == -1) {
    throw std::runtime_error("`TcpServer::bindAndListen` bind failed.");
  }

  if (listen() == -1) {
    throw std::runtime_error("`TcpServer::bindAndListen` listen failed.");
  }
}

int TcpServer::setVar(decltype(ReuseAddr)) {
  int buf = 1;
  return setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, &buf, sizeof(buf));
}

int TcpServer::setVar(decltype(ReusePort)) {
  int buf = 1;
  return setsockopt(fd(), SOL_SOCKET, SO_REUSEPORT, &buf, sizeof(buf));
}

}