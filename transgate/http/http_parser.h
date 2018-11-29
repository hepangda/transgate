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

#ifndef TRANSGATE_HTTP_PARSER_H
#define TRANSGATE_HTTP_PARSER_H

#include <memory>

#include "../base/noncopyable.h"
#include "http_types.h"
#include "../utils/readable_buffer.h"

namespace tg {

class HttpRequest;

enum HttpParserState {
  kHPSBegin,

  kHPSMetOptions,
  kHPSMetHead,
  kHPSMetGet,
  kHPSMetPost,
  kHPSMetAlmostDone,
  kHPSMetDone,

  kHPSUriStart,
  kHPSUri,
  kHPSUriEnd,

  kHPSVerHttpSlash,
  kHPSVerMajor,
  kHPSVerDot,
  kHPSVerMinor,
  kHPSVerEnd,

  kHPSCR,
  kHPSCRLF,
  kHPSCRLFCR,
  kHPSCRLFCRLF,

  kHPSHeader,
  kHPSColon,
  kHPSValue,
  kHPSDied,
};

enum HttpParserErrors {
  // Not Finished
  kHPEFine,
  kHPENotAvailable,

  // Parsed, but excepted content
  kHPEExceptedContent,

  // Finished
  kHPEParsed,   // finish without errors
  kHPEInvalidMethod,
  kHPEInvalidUri,
  kHPEInvalidVersion,
  kHPEInvalidContentLength,
  kHPEUnsupportedVersion,
  kHPECRLF,
  kHPEInvalidHeader,
  kHPEUnrecognizedChar,
  kHPEUnexceptedEnd,
  kHPEEntityTooLarge,
};

struct HttpParserBuffer {
  HttpParserState state = kHPSBegin;
  HttpParserErrors err = kHPEFine;
  HttpMethod method = kHMInvalid;

  const char *uri_starts = nullptr;
  const char *header_starts = nullptr;
  const char *value_starts = nullptr;

  int uri_length = 1;
  int header_length = 1;
  int value_length = 1;
  int content_length = -1;

  int ver_major = 0;
  int ver_minor = 0;

  bool keep_alive = true;
  bool keep_alive_set = false;
};

class HttpParser : public Noncopyable {
 public:
  HttpParser(std::shared_ptr<ReadableBuffer> stream, std::shared_ptr<HttpRequest> request)
      : stream_(std::move(stream)), request_(std::move(request)) {}

  HttpParserErrors doParse();
  bool isParsed() const;
  bool isFinished() const;
 private:
  bool parseOnce();
  void setParseResult();
  bool parseable();

  std::shared_ptr<ReadableBuffer> stream_;
  std::shared_ptr<HttpRequest> request_;
  HttpParserBuffer f_;
};

}

#endif // TRANSGATE_HTTP_PARSER_H
