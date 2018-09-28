//
// Created by pangda on 8/22/18.
//

#ifndef TRANSGATE_BUFFER_H
#define TRANSGATE_BUFFER_H

namespace tg {

class Buffer {
 public:
  virtual const char *rptr() const = 0;
  virtual int length() const = 0;
  virtual int readable() const = 0;
  virtual int writeable() const = 0;
  virtual void doRead(int bytes) = 0;
};

}

#endif //TRANSGATE_BUFFER_H
