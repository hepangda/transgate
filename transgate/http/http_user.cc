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

#include "http_user.h"

namespace tg {

HttpUser::~HttpUser() {
  user_->close();
}

int HttpUser::fd() const { return user_->fd(); }

void HttpUser::onRead() {
  prepare(Preparation::ReadBuffer);

  for (int ret; (ret = user_->read(read_buffer_)) != -1;) {}

  try_consume();
}

void HttpUser::try_consume() {
  prepare(Preparation::Request);
  prepare(Preparation::Parser);
  prepare(Preparation::WriteLoop);

  parser_->doParse();
  if (parser_->isFinished()) {
    prepare(Preparation::Provider);

    provider_->provide();
  }

  write_loop_->doAll();
}

void HttpUser::prepare(HttpUser::Preparation what) {
  switch (what) {
  case Preparation::ReadBuffer:
    if (!read_buffer_) read_buffer_ = std::make_shared<CharBuffer>(2048);
    break;
  case Preparation::WriteLoop:
    if (!write_loop_) write_loop_ = std::make_shared<WriteLoop>(fd(), 2048);
    break;
  case Preparation::Request:
    if (!request_) request_ = std::make_shared<HttpRequest>();
    break;
  case Preparation::Provider:
    if (!provider_) provider_ = std::make_unique<HttpProvider>(write_loop_); //todo: finish it
    break;
  case Preparation::Parser:
    if (!parser_) parser_ = std::make_unique<HttpParser>(read_buffer_, request_);
    break;
  }
}

}