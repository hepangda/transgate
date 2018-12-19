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

#include "tcp_socket.h"

#include <unistd.h>
#include <fcntl.h>

namespace tg {

int TcpSocket::read(void *buffer, int length, int flags) const {
  return static_cast<int>(::recv(socket_fd_, buffer, static_cast<size_t >(length), flags));
}

int TcpSocket::read(std::shared_ptr<CharBuffer> &buffer, int length, int flags) const {
  if (length == -1 || length > buffer->writeable()) {
    length = buffer->writeable();
  }

  int read_bytes = static_cast<int>(::recv(socket_fd_, buffer->writeptr(), static_cast<size_t>(length), flags));
  if (read_bytes != -1) {
    buffer->write(read_bytes);
  }

  return read_bytes;
}

int TcpSocket::write(void *buffer, int length, int flags) const {
  return static_cast<int>(::send(socket_fd_, buffer, static_cast<size_t>(length), flags));
}

int TcpSocket::fd() const {
  return socket_fd_;
}

int TcpSocket::close() {
  int ret = ::close(socket_fd_);
  if (ret != -1) {
    socket_fd_ = -1;
  }
  return ret;
}

int TcpSocket::setNonblockAndCloseExec() {
  int flags = ::fcntl(socket_fd_, F_GETFL, 0);
  flags = flags | O_NONBLOCK | FD_CLOEXEC;
  return ::fcntl(socket_fd_, F_SETFL, flags);
}

int TcpSocket::setNonblock() {
  int flags = ::fcntl(socket_fd_, F_GETFL, 0);
  flags = flags | O_NONBLOCK;
  return ::fcntl(socket_fd_, F_SETFL, flags);
}

}