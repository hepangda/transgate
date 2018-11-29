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
#include "../base/linuxfile.h"

namespace tg {

class CharBuffer;
class WriteLoop;
class HttpRequest;
class HttpProvider;
class HttpParser;
class TcpSocket;

class HttpUser : public Noncopyable, public LinuxFile {
 public:
  explicit HttpUser(std::unique_ptr<TcpSocket> &&user) { user.swap(user_); }
  explicit HttpUser(int fd) { user_ = std::make_unique<TcpSocket>(fd); }
  virtual ~HttpUser();

  int fd() const final;

  void onRead();
  void try_consume();
 private:
  std::shared_ptr<CharBuffer> read_buffer_ = nullptr;
  std::shared_ptr<WriteLoop> write_loop_ = nullptr;

  std::shared_ptr<HttpRequest> request_ = nullptr;
  std::unique_ptr<HttpProvider> provider_ = nullptr;
  std::unique_ptr<HttpParser> parser_ = nullptr;

  std::unique_ptr<TcpSocket> user_;

  enum class Preparation {
    ReadBuffer,
    WriteLoop,
    Request,
    Provider,
    Parser,
  };
  void prepare(Preparation what);
};

struct HttpUserHash {
  size_t operator()(const HttpUser &http_user) {
    return std::hash<int>()(http_user.fd());
  }
};

};

#endif // TRANSGATE_HTTP_USER_H
