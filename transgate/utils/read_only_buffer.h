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


#ifndef TRANSGATE_READ_ONLY_BUFFER_H
#define TRANSGATE_READ_ONLY_BUFFER_H

namespace tg {

/* interface */ class ReadOnlyBuffer {
 public:
  virtual int readable() const = 0;
  virtual const char *readptr() const = 0;
  virtual int read(void *dest, int bytes) = 0;
  virtual int read(int bytes) = 0;
  virtual char peek() const = 0;
  virtual char peek(int nums) const = 0;
  virtual int size() const = 0;
};

}

#endif // TRANSGATE_READ_ONLY_BUFFER_H
