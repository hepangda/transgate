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

#ifndef TRANSGATE_HTTP_USER_H
#define TRANSGATE_HTTP_USER_H

#include <memory>

#include "../base/noncopyable.h"
#include "../utils/char_buffer.h"
#include "http_parser.h"
#include "../net/write_loop.h"
#include "../base/linuxfile.h"
#include "../net/tcp_socket.h"

namespace tg {

class HttpUser : public Noncopyable, public LinuxFile {
 public:
  explicit HttpUser(std::unique_ptr<TcpSocket> &&user) { user.swap(user_); }
  explicit HttpUser(int fd) { user_ = std::make_unique<TcpSocket>(fd); }
  int fd() const final { return user_->fd(); }

  void onRead() {
    if (read_buffer_ == nullptr) {
      read_buffer_ = std::make_shared<CharBuffer>(2048); // todo: fix magic number
    }
    for (int ret; (ret = user_->read(read_buffer_)) != -1;) {}

    try_consume();
  }

  void try_consume() {
    if (write_buffer_ == nullptr) {
      write_buffer_ = std::make_shared<CharBuffer>(2048); // todo: fix magic number
    }

    write_buffer_->write("HTTP/1.1 200 OK\r\nConnection: Close\r\nContent-Length: 260\r\n\r\n<html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>"
                         "<html></html><html></html><html></html><html></html><html></html><html></html>");


  }
 private:
  // lazy-init
  std::shared_ptr<CharBuffer> read_buffer_ = nullptr;
  std::shared_ptr<CharBuffer> write_buffer_ = nullptr;
  std::unique_ptr<HttpParser> parser_ = nullptr;
  std::unique_ptr<WriteLoop> write_loop_ = nullptr;

  std::unique_ptr<TcpSocket> user_;
};

struct HttpUserHash {
  size_t operator()(const HttpUser &http_user) {
    return std::hash<int>()(http_user.fd());
  }
};

}

#endif // TRANSGATE_HTTP_USER_H
