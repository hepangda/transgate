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

#ifndef TRANSGATE_USER_H
#define TRANSGATE_USER_H

#include <memory>

#include "../base/noncopyable.h"
#include "../base/linuxfile.h"
#include "../net/tcp_socket.h"
#include "../net/write_loop.h"
#include "../http/http_request.h"
#include "content_provider.h"
#include "../http/http_parser.h"
#include "../net/epoll_event.h"
#include "../utils/generic_buffer.h"

namespace tg {

class User : public Noncopyable, public LinuxFile {
 public:
  explicit User(std::unique_ptr<TcpSocket> &&user, long ts) : time_stamp_(ts) { user.swap(user_); }
  explicit User(int fd, long ts): time_stamp_(ts), user_(std::make_unique<TcpSocket>(fd)) { }
  virtual ~User();

  int fd() const final;

  void onRead();
  void onWrite();
  long time_stamp() const { return time_stamp_; }
  void touch(long time) { time_stamp_ = time; }

  std::shared_ptr<GenericBuffer> get_interaction_buffer() const { return interaction_buffer_; }

  bool closeable();
  EpollEventType type();
 private:
  std::shared_ptr<CharBuffer> read_buffer_ = nullptr;
  std::shared_ptr<WriteLoop> write_loop_ = nullptr;

  std::shared_ptr<HttpRequest> request_ = nullptr;
  std::unique_ptr<ContentProvider> provider_ = nullptr;
  std::unique_ptr<HttpParser> parser_ = nullptr;

  std::unique_ptr<TcpSocket> user_;
  std::shared_ptr<GenericBuffer> interaction_buffer_;
  long time_stamp_;
  void prepare();
};

};

#endif // TRANSGATE_USER_H
