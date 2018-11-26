//
// Created by pangda on 11/26/18.
//

#ifndef TRANSGATE_WRITE_LOOP_H
#define TRANSGATE_WRITE_LOOP_H

#include <deque>
#include <functional>

namespace tg {

enum WriteEventType : int {
  kWETSend,
  kWETSendfile,
};

struct WriteEvent {
  WriteEventType type;
  int offset;
  int length;
};

struct WriteEventResult {
  int next_offset;
  int next_length;
};

// TODO: finish it
class WriteLoop {
 public:
  bool doOnce() {
    auto f = q_.front();
    q_.pop_front();

//    if (f)
return false;
  }

  void doUntilWouldBlock() {

  }

 private:
  std::deque<std::function<WriteEventResult()>> q_;
};

}

#endif // TRANSGATE_WRITE_LOOP_H
