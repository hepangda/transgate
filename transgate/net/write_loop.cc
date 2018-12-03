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

#include "write_loop.h"

#include <cstdarg>
#include <sys/socket.h>
#include <sys/sendfile.h>

#include "../utils/file_reader.h"

namespace tg {

int WriteLoop::swrite(const char *format, ...) {
  va_list va;
  va_start(va, format);
  int ret = buffer_->swrite(format, va);
  va_end(va);
  write(ret);
  return ret;
}

bool WriteLoop::doOnce() {
  if (q_.empty()) return false;

  auto f = q_.front();
  q_.pop_front();

  return f();
}

bool WriteLoop::actSend(int length) {
  // TODO: prevent length < 0
  int sent = 0;

  // TODO: deal if errno != EINTR || EAGAIN
  for (long r; (r = ::send(fd_, buffer_->readptr(), static_cast<size_t>(length - sent), 0)) > 0;) {
    sent += r;
  }

  buffer_->read(sent);

  int rest = length - sent;
  if (rest > 0) {
    insertSend(rest);
    return false;
  }
  return true;
}

bool WriteLoop::actSendfile(std::shared_ptr<FileReader> file) {
  // TODO: deal if errno != EINTR || EAGAIN
  for (ssize_t r; (r = file->sendfile(fd_)) > 0; ) { }

  if (!file->isDone()) {
    insertSendfile(file);
    return false;
  }
  return true;
}

}