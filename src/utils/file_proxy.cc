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

#include "file_proxy.h"

#include <unistd.h>
#include <fcntl.h>

namespace tg {

FileProxy::FileProxy(const char *path) : fd_(open(path, O_RDONLY)) {}
FileProxy::FileProxy(const FileProxy &directory, const char *path) : fd_(openat(directory.fd(), path, O_RDONLY)) {}

FileProxy::~FileProxy() {
  close(fd_);
}

off_t FileProxy::size() {
  prepareStat();
  return stat_->st_size;
}

bool FileProxy::isDirectory() {
  prepareStat();
  return (stat_->st_mode & S_IFDIR) != 0;
}

void FileProxy::prepareStat() {
  if (!stat_) {
    stat_ = std::make_unique<struct stat>();
    fstat(fd_, stat_.get());
  }
}

}