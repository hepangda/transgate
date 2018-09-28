//
// Created by pangda on 9/4/18.
//

#ifndef TRANSGATE_HTTP_USER_H
#define TRANSGATE_HTTP_USER_H

#include <memory>

#include "../base/noncopyable.h"
#include "../utils/heap_buffer.h"

namespace tg {

class HttpUser : public Noncopyable {
 public:
  int fd() const { return user_fd; }
 private:
  // lazy-init buffer
  std::unique_ptr<HeapBuffer> buffer = nullptr;
  int user_fd;
};

struct HttpUserHash {
  size_t operator()(const HttpUser &http_user) {
    return std::hash<int>()(http_user.fd());
  }
};

}

#endif //TRANSGATE_HTTP_USER_H
