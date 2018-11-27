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
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "../interface/linuxfile.hpp"

#include "../utils/debug.hpp"
namespace tg {

class ROFile : public ILinuxFile {
 private:
  int _fd = -1;
  struct stat _stat;

 public:
  ROFile() {}
  ROFile(char *path) {
    _fd = open(path, O_RDONLY);
    if (_fd != -1) {
      if (fstat(_fd, &_stat) == -1) {
        close(_fd);
        _fd = -1;
      }
    }
  }

  ROFile(const std::string &path) : ROFile((char *)path.c_str()) {}

  virtual ~ROFile() {
    if (_fd != -1) {
      close(_fd);
    }
  }

  inline int sendTo(ILinuxFile &sock, size_t count, off_t *offset = nullptr) {
    return sendfile(sock.getFd(), _fd, offset, count);
  }

  inline bool ok() const noexcept { return _fd != -1; }

  inline off_t getSize() {
    if (ok()) {
      return _stat.st_size;
    }
    return 0;
  }

  int getFd() override { return _fd; }
};

}  // namespace tg