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

#ifndef TRANSGATE_FILE_READER_H
#define TRANSGATE_FILE_READER_H

#include <memory>

#include "file_proxy.h"

namespace tg {

class FileReader : public FileProxy {
 public:
  explicit FileReader(const char *path): FileProxy(path) {}
  FileReader(FileProxy &directory, const char *path): FileProxy(directory, path) {}
  FileReader(FileProxy &directory, const std::string &path): FileProxy(directory, path.c_str()) {}

  long sendfile(int socket_fd);
  bool isDone() const { return length_ == offset_; }
 private:
  long length_ = size();
  long offset_ = 0;
};

}

#endif // TRANSGATE_FILE_READER_H
