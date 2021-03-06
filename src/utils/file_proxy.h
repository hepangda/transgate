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

#ifndef TRANSGATE_FILE_PROXY_H
#define TRANSGATE_FILE_PROXY_H

#include <memory>

#include <sys/stat.h>

#include "../base/linuxfile.h"
#include "../base/noncopyable.h"

namespace tg {

class FileProxy: public Noncopyable, public LinuxFile {
 public:
  explicit FileProxy(const char *path);
  FileProxy(const FileProxy &directory, const char *path);
  virtual ~FileProxy();

  int fd() const final { return fd_; }
  bool good() const { return fd_ > 0; }
  long size();
  bool isDirectory();
 private:
  int fd_;
  std::unique_ptr<struct stat> stat_ = nullptr;

  void prepareStat();
};

}

#endif // TRANSGATE_FILE_PROXY_H
