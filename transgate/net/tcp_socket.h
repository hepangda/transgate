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

namespace tg {

class Buffer;
class HeapBuffer;

class TcpSocket: public Copyable, public LinuxFile {
 public:
  explicit TcpSocket(int fd) : socket_fd_(fd) {}
  TcpSocket(): socket_fd_(socket(AF_INET, SOCK_STREAM, 0)) {}

  int read(void *buffer, int length, int flags = 0) const;
  int read(HeapBuffer &buffer, int length = -1, int flags = 0) const;

  int write(void *buffer, int length, int flags = 0) const;
  int write(Buffer &buffer, int length = -1, int flags = 0) const;
  int write(Buffer &&buffer, int flags = 0) const;

  int setNonblockAndCloseExec();
  int setNonblock();

  int close() const;
  int fd() const override;
 private:
  int socket_fd_;
};

}

#endif //TRANSGATE_TCP_SOCKET_H
