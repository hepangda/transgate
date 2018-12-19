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

#ifndef TRANSGATE_TCP_SOCKET_H
#define TRANSGATE_TCP_SOCKET_H

#include <sys/socket.h>

#include "../base/linuxfile.h"
#include "../base/copyable.h"
#include "../utils/char_buffer.h"
#include <memory>

namespace tg {

class TcpSocket : public Copyable, public LinuxFile {
 public:
  explicit TcpSocket(int fd) : socket_fd_(fd) {}
  explicit TcpSocket(const LinuxFile &linux_file) : socket_fd_(linux_file.fd()) {}
  TcpSocket() : socket_fd_(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)) {}
  virtual ~TcpSocket() { if (socket_fd_ == -1) close(); }

  int read(void *buffer, int length, int flags = 0) const;
  int read(std::shared_ptr<CharBuffer> &buffer, int length = -1, int flags = 0) const;

  int write(void *buffer, int length, int flags = 0) const;

  int setNonblockAndCloseExec();
  int setNonblock();

  int close();
  int fd() const override;
 private:
  int socket_fd_ = -1;
};

}

#endif // TRANSGATE_TCP_SOCKET_H
