/****************************************************************************
 * Copyright 2018 hepangda                                                  *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *     http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/
#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include "../interface/linuxfile.hpp"
#include "bytes.hpp"
#include "debug.hpp"
#include "rofile.hpp"

namespace tg {

enum class NetworkDomain { IPV4 = AF_INET, IPV6 = AF_INET6 };

enum class NetworkType {
  STREAM = SOCK_STREAM,
  DATAGRAM = SOCK_DGRAM,
};

enum class NetworkWFlag : int {
  NOSIGNAL = ::MSG_NOSIGNAL,
  OUT_OF_BAND = ::MSG_OOB,
};

enum class NetworkRFlag : int {
  PEEK = ::MSG_PEEK,
  WAITALL = ::MSG_WAITALL,
  OUT_OF_BAND = ::MSG_OOB,
};

inline int operator|(const NetworkRFlag lhs, const NetworkRFlag rhs) {
  return static_cast<int>(lhs) | static_cast<int>(rhs);
}

inline int operator|(const NetworkWFlag lhs, const NetworkWFlag rhs) {
  return static_cast<int>(lhs) | static_cast<int>(rhs);
}

class Socket : public ILinuxFile {
 protected:
  int _sktfd = -1;

 public:
  Socket(NetworkType type = NetworkType::STREAM,
         NetworkDomain dom = NetworkDomain::IPV4) {
          
    // Specifying the last parameter `protocol` of 0 means to use default
    // protocol which is appropriate for the requested socket type.
    _sktfd = socket(static_cast<int>(dom), static_cast<int>(type), 0);
  }

  Socket(const Socket &skt) { _sktfd = skt._sktfd; }

  Socket(const int fd) { _sktfd = fd; }

  inline void close() { ::close(_sktfd); }

  // inline int write(ByteBuffer &buf, size_t len, std::vector<int> flags = {})
  // {
  //   return ::send(_sktfd, buf.pointer(), len, flags);
  // }

  // inline int write(void *buf, size_t len, std::vector<int> flags = {}) {
  //   return ::send(_sktfd, buf, len, flags);
  // }

  inline int write(ByteBuffer &buf, size_t len, int flags = 0)
  {
    return ::send(_sktfd, buf.pointer(), len, flags);
  }

  inline int write(void *buf, size_t len, int flags = 0) {
    return ::send(_sktfd, buf, len, flags);
  }

  inline int write(ROFile &file, off_t *offset, size_t len, int flags = 0) {
    long rest = len;
    int ret = 0;
    while (rest > 0) {
      auto r = sendfile(_sktfd, file.getFd(), offset, len);
      if (r == -1) break;
      ret += r;
      rest -= r;
    }
    return ret;
  }

  inline int read(tg::ByteBuffer &buf, int len, int flags = 0) {
    return ::recv(_sktfd, buf.pointer(), len, flags);
  }

  inline int read(void *buf, size_t len, int flags = 0) {
    return ::recv(_sktfd, buf, len, flags);
  }

  inline int getFd() override { return _sktfd; }
};

class Address {
 private:
  ::sockaddr_in _addr;
  Address() {}

 public:
  Address(const char *addr, unsigned int port) {
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = (sa_family_t)NetworkDomain::IPV4;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(addr);

    if (inet_pton(int(NetworkDomain::IPV4), addr, &_addr.sin_addr) <= 0) {
      throw std::runtime_error("Cannot convert address");
    }
  }

  Address(unsigned int port) {
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = (sa_family_t)NetworkDomain::IPV4;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }

  static Address spawn() { return std::move(Address()); }

  sockaddr *pointer() { return reinterpret_cast<sockaddr *>(&_addr); }

  template <typename T = socklen_t>
  constexpr socklen_t length() const {
    return static_cast<T>(sizeof(_addr));
  }
};

class ClientSocket : public Socket {
 private:
  Address _cltaddr = Address::spawn();

 public:
  ClientSocket(int fd) : Socket(fd) {}
  ClientSocket(int fd, Address adr) : Socket(fd), _cltaddr(adr) {}
};

class ServerSocket : public Socket {
 private:
  Address _addr;

 public:
  ServerSocket() : ServerSocket(0) {}
  ServerSocket(Address addr) : _addr(addr) {}

  inline std::pair<ClientSocket, Address> acceptWithAddress() {
    Address cltaddr = Address::spawn();
    socklen_t len = cltaddr.length();
    int ret = ::accept(_sktfd, cltaddr.pointer(), &len);
    return std::make_pair(ret, cltaddr);
  }

  inline ClientSocket accept() { return acceptWithAddress().first; }

  inline int bind() {
    int ret = ::bind(_sktfd, _addr.pointer(), _addr.length());
    if (ret < 0) {
      throw std::runtime_error("Cannot bind address.");
    }

    return ret;
  }

  inline int setReusePortVar() {
    int buf = 1;
    return setsockopt(_sktfd, SOL_SOCKET, SO_REUSEPORT, &buf, sizeof(buf));
  }

  inline int listen() {
    int ret = ::listen(_sktfd, 100);
    if (ret < 0) {
      throw std::runtime_error("Cannot listen.");
    }

    return ret;
  }

  inline void bindAndListen() {
    bind();
    listen();
  }
};

}  // namespace tg