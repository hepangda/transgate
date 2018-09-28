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

#include <unistd.h>
#include <fcntl.h>

#include "../utils/buffer.h"
#include "../utils/heap_buffer.h"
#include "tcp_socket.h"

namespace tg {

int TcpSocket::read(void *buffer, int length, int flags) const {
  return static_cast<int>(::recv(socket_fd_, buffer, static_cast<size_t >(length), flags));
}

int TcpSocket::read(tg::HeapBuffer &buffer, int length, int flags) const {
  if (length == -1 || length > buffer.readable()) {
    length = buffer.writeable();
  }

  int readed_bytes = static_cast<int>(::recv(socket_fd_, buffer.wptr(), static_cast<size_t>(length), flags));
  if (readed_bytes != -1) {
    buffer.doWrite(readed_bytes);
  }

  return readed_bytes;
}

int TcpSocket::write(void *buffer, int length, int flags) const {
  return static_cast<int>(::send(socket_fd_, buffer, static_cast<size_t>(length), flags));
}

int TcpSocket::write(tg::Buffer &buffer, int length, int flags) const {
  if (length == -1 || length > buffer.writeable()) {
    length = buffer.readable();
  }

  int written_bytes = static_cast<int>(::send(socket_fd_, buffer.rptr(), static_cast<size_t>(length), flags));
  if (written_bytes != -1) {
    buffer.doRead(written_bytes);
  }

  return written_bytes;
}

int TcpSocket::write(tg::Buffer &&buffer, int flags) const {
  return static_cast<int>(::send(socket_fd_, buffer.rptr(), static_cast<size_t>(buffer.readable()), flags));
}

int TcpSocket::fd() const {
  return socket_fd_;
}

int TcpSocket::close() const {
  return ::close(socket_fd_);
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