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

#include "user.h"

namespace tg {

User::~User() {
  user_->close();
}

int User::fd() const { return user_->fd(); }

EpollEventType User::type() {
  return write_loop_->empty() ? ETEOReadable() : ETEOAllof();
}

void User::onRead() {
  prepare();

  for (int ret; (ret = user_->read(read_buffer_)) != -1;) {}

  parser_->doParse();
  if (parser_->isFinished()) {
    provider_->provide();
    parser_->clear();
  }

  write_loop_->doAll();
}

void User::onWrite() {
  prepare();
  write_loop_->doAll();
}

void User::prepare() {
  if (!read_buffer_) read_buffer_ = std::make_shared<CharBuffer>(2048);
  if (!write_loop_) write_loop_ = std::make_shared<WriteLoop>(fd(), 2048);
  if (!request_) request_ = std::make_shared<HttpRequest>();
  if (!provider_) provider_ = std::make_unique<ContentProvider>(write_loop_, request_); //todo: finish it
  if (!parser_) parser_ = std::make_unique<HttpParser>(read_buffer_, request_);
}

bool User::closeable() {
  // todo: 长时间没有响应了，那么也关闭

  if (!request_) return false;
  return parser_->isFinished() && !request_->isKeepalive() && write_loop_->empty();
}

}