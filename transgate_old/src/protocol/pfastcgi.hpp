/*
  MIT License

  Copyright (c) 2018 hepangda

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/
#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <new>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define CASTUC(val) static_cast<unsigned char>((val))

enum class FcgiType : int {
  BEGIN = 1,
  ABORT = 2,
  END = 3,
  PARAMS = 4,
  STDIN = 5,
  STDOUT = 6,
  STDERR = 7,
  DATA = 8,
  GETVAL = 9,
  VALRES = 10,
  UNKNOWN = 11
};

enum class FcgiRole { RESPONSER = 1, AUTHORIZER = 2, FILTER = 3 };

struct FcgiPreread {
  FcgiType type;
  int requestId;
  int contentLength;
  int paddingLength;
};

class FcgiManager {
 private:
  int _reqid = -1;
  static const int FcgiVersion = 1;
  static const int FcgiKeepAlive = 1;

 public:
  struct FcgiHeader {
    unsigned char version;
    unsigned char type;
    unsigned char requestIdB1;
    unsigned char requestIdB0;
    unsigned char contentLengthB1;
    unsigned char contentLengthB0;
    unsigned char paddingLength;
    unsigned char reserved;

    FcgiHeader() {}
    FcgiHeader(FcgiType reqtype, int ctlen, int reqid = 1, int padlen = 0) {
      version = CASTUC(FcgiVersion);
      type = CASTUC(reqtype);
      requestIdB1 = CASTUC((reqid >> 8) & 0xff);
      requestIdB0 = CASTUC(reqid & 0xff);
      contentLengthB1 = CASTUC((ctlen >> 8) & 0xff);
      contentLengthB0 = CASTUC(ctlen & 0xff);
      paddingLength = padlen;
      reserved = 0;
    }
  };

  struct FcgiBodyReqBegin {
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];

    FcgiBodyReqBegin() {}
    FcgiBodyReqBegin(int role, bool keepAlive) {
      roleB1 = CASTUC((role >> 8) & 0xff);
      roleB0 = CASTUC(role & 0xff);
      flags = keepAlive ? FcgiKeepAlive : 0;
      for (int i = 0; i < 5; i++) {
        reserved[i] = 0;
      }
    }
  };

  struct FcgiMsgBegin {
    FcgiHeader header;
    FcgiBodyReqBegin body;
  };

  struct FcgiBodyReqEnd {
    unsigned char appStatusB3;
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;
    unsigned char reserved[3];
  };

  struct FcgiBodyParamsHeader {
    unsigned char nameLengthB3;
    unsigned char nameLengthB2;
    unsigned char nameLengthB1;
    unsigned char nameLengthB0;
    unsigned char valueLengthB3;
    unsigned char valueLengthB2;
    unsigned char valueLengthB1;
    unsigned char valueLengthB0;

    FcgiBodyParamsHeader(int nlen, int vlen) {
      nameLengthB3 = CASTUC((nlen >> 24) | 0x80);
      nameLengthB2 = CASTUC(nlen >> 16);
      nameLengthB1 = CASTUC(nlen >> 8);
      nameLengthB0 = CASTUC(nlen);
      valueLengthB3 = CASTUC((vlen >> 24) | 0x80);
      valueLengthB2 = CASTUC(vlen >> 16);
      valueLengthB1 = CASTUC(vlen >> 8);
      valueLengthB0 = CASTUC(vlen);
    }
  };

 protected:
  int _fcgifd = -1;
  inline void closeSocket() {
    if (_fcgifd != -1) {
      close(_fcgifd);
    }
  }

 public:
  virtual int start(const char *addr, unsigned int port) = 0;

  FcgiManager(int reqid) : _reqid(reqid) {}
  virtual ~FcgiManager() { closeSocket(); }

  int beginRequest(FcgiRole role = FcgiRole::RESPONSER, bool keepAlive = false,
                   int reqid = 1) const {
    static constexpr auto reqLen = sizeof(FcgiBodyReqBegin);
    FcgiMsgBegin msg = {{FcgiType::BEGIN, reqLen, reqid},
                        {int(role), keepAlive}};
    return doWrite(&msg, sizeof(FcgiMsgBegin));
  }

  int sendParams(const char *key, const char *value, int reqid = 1) const {
    static constexpr auto headLen =
        sizeof(FcgiBodyParamsHeader) + sizeof(FcgiHeader);
    auto klen = strlen(key), vlen = strlen(value),
         allen = headLen + klen + vlen;

    std::shared_ptr<unsigned char> buf(new unsigned char[allen],
                                       std::default_delete<unsigned char[]>());

    new (buf.get())
        FcgiHeader(FcgiType::PARAMS, allen - sizeof(FcgiHeader), reqid);
    new (buf.get() + sizeof(FcgiHeader)) FcgiBodyParamsHeader(klen, vlen);
    memcpy(buf.get() + headLen, key, klen);
    memcpy(buf.get() + headLen + klen, value, vlen);

    return doWrite(buf.get(), allen);
  }

  int endParams(int reqid = 1) const {
    static constexpr auto reqLen = 0;
    FcgiHeader msg = {FcgiType::PARAMS, 0, reqid};
    return doWrite(&msg, sizeof(FcgiHeader));
  }

  bool isOk() const { return _fcgifd != -1; }

  FcgiPreread preRead() {
    FcgiHeader head;
    doRead(&head, sizeof(FcgiHeader));

    return {FcgiType(head.type), (head.requestIdB1 << 8) + head.requestIdB0,
            (head.contentLengthB1 << 8) + head.contentLengthB0,
            head.paddingLength};
  }

  // -1 represents END OF REQUEST
  // 0  represents NO
  // 1  represents YES
  int pkgType(FcgiPreread fp) {
    if (fp.type == FcgiType::END) return -1;
    if (fp.type != FcgiType::STDOUT && fp.type != FcgiType::STDERR) return 0;
    return (fp.contentLength > 0) ? 1 : 0;
  }

  inline int readContent(FcgiPreread pre, void *buffer) {
    std::shared_ptr<char> padding(new char[pre.paddingLength],
                                  std::default_delete<char[]>());
    return readContent(pre, buffer, padding.get());
  }

  int readContent(FcgiPreread pre, void *buffer, void *paddingBuffer) {
    int res = doRead(buffer, pre.contentLength);
    doRead(paddingBuffer, pre.paddingLength);
    return res;
  }

  inline int doRead(void *buf, size_t length, int flags = 0) const {
    return ::recv(_fcgifd, buf, length, flags);
  }

  inline int doWrite(void *buf, size_t length, int flags = 0) const {
    return ::send(_fcgifd, buf, length, flags);
  }
};

class FcgiManagerINET : public FcgiManager {
 public:
  FcgiManagerINET(const char *addr, const unsigned int port, int reqid = 1)
      : FcgiManager(reqid) {
    start(addr, port);
  }
  int start(const char *addr, unsigned int port) {
    closeSocket();

    sockaddr_in sktaddr;
    memset(&sktaddr, 0, sizeof(sockaddr_in));
    sktaddr.sin_family = AF_INET;
    sktaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, addr, &sktaddr.sin_addr) <= 0) {
      return -1;
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    _fcgifd = fd;
    return connect(fd, (sockaddr *)&sktaddr, sizeof(sockaddr_in));
  }
};

class FcgiManagerUnix : public FcgiManager {
 public:
  FcgiManagerUnix(const char *path, int reqid = 1) : FcgiManager(reqid) {
    start(path);
  }
  int start(const char *path, unsigned int padding = 0) {
    closeSocket();

    sockaddr_un sktaddr;
    memset(&sktaddr, 0, sizeof(sockaddr_un));
    sktaddr.sun_family = AF_UNIX;
    strcpy(sktaddr.sun_path, path);

    int fd = socket(AF_UNIX, SOCK_STREAM, 0),
        size = offsetof(sockaddr_un, sun_path) + strlen(sktaddr.sun_path);
    _fcgifd = fd;
    return connect(fd, (sockaddr *)&sktaddr, size);
  }
};

#undef CASTUC