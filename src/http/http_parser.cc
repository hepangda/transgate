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
#include "http_parser.h"

#include "http_request.h"
#include "../core/config_provider.h"

namespace tg {

namespace {
inline bool isheader(char c) {
  return isalnum(c) || c == '-' || c == '_';
}

inline bool isuri(char c) {
  static bool isuri[] = {
/*0   nul    soh    stx    etx    eot    enq    ack    bel     7*/
      false, false, false, false, false, false, false, false,
/*8   bs     ht     nl     vt     np     cr     so     si     15*/
      false, false, false, false, false, false, false, false,
/*16  dle    dc1    dc2    dc3    dc4    nak    syn    etb    23*/
      false, false, false, false, false, false, false, false,
/*24  can    em     sub    esc    fs     gs     rs     us     31*/
      false, false, false, false, false, false, false, false,
/*32  ' '    !      "      #     $     %     &     '          39*/
      false, false, false, true, true, true, true, false,
/*40  (      )      *      +     ,     -     .     /          47*/
      false, false, false, true, true, true, true, true,
/*48  0     1     2     3     4     5     6     7             55*/
      true, true, true, true, true, true, true, true,
/*56  8     9     :     ;     <      =     >      ?           63*/
      true, true, true, true, false, true, false, true,
/*64  @     A     B     C     D     E     F     G             71*/
      true, true, true, true, true, true, true, true,
/*72  H     I     J     K     L     M     N     O             79*/
      true, true, true, true, true, true, true, true,
/*80  P     Q     R     S     T     U     V     W             87*/
      true, true, true, true, true, true, true, true,
/*88  X     Y     Z     [      \      ]      ^      _         95*/
      true, true, true, false, false, false, false, true,
/*96  `      a     b     c     d     e     f     g           103*/
      false, true, true, true, true, true, true, true,
/*104 h     i     j     k     l     m     n     o            113*/
      true, true, true, true, true, true, true, true,
/*112 p     q     r     s     t     u     v     w            119*/
      true, true, true, true, true, true, true, true,
/*120 x     y     z     {      |      }      ~      del      127*/
      true, true, true, false, false, false, false, false
  };

  return (c >= 0) ? isuri[c] : false;
}

inline bool isvalue(char c) {
  static bool isvalue[] = {
/*0   nul    soh    stx    etx    eot    enq    ack    bel     7*/
      false, false, false, false, false, false, false, false,
/*8   bs     ht     nl     vt     np     cr     so     si     15*/
      false, false, false, false, false, false, false, false,
/*16  dle    dc1    dc2    dc3    dc4    nak    syn    etb    23*/
      false, false, false, false, false, false, false, false,
/*24  can    em     sub    esc    fs     gs     rs     us     31*/
      false, false, false, false, false, false, false, false,
/*32  ' '    !      "      #     $     %     &     '          39*/
      true, true, true, true, true, true, true, true,
/*40  (      )      *      +     ,     -     .     /          47*/
      true, true, true, true, true, true, true, true,
/*48  0     1     2     3     4     5     6     7             55*/
      true, true, true, true, true, true, true, true,
/*56  8     9     :     ;     <      =     >      ?           63*/
      true, true, true, true, true, true, true, true,
/*64  @     A     B     C     D     E     F     G             71*/
      true, true, true, true, true, true, true, true,
/*72  H     I     J     K     L     M     N     O             79*/
      true, true, true, true, true, true, true, true,
/*80  P     Q     R     S     T     U     V     W             87*/
      true, true, true, true, true, true, true, true,
/*88  X     Y     Z     [      \      ]      ^      _         95*/
      true, true, true, true, true, true, true, true,
/*96  `      a     b     c     d     e     f     g           103*/
      true, true, true, true, true, true, true, true,
/*104 h     i     j     k     l     m     n     o            113*/
      true, true, true, true, true, true, true, true,
/*112 p     q     r     s     t     u     v     w            119*/
      true, true, true, true, true, true, true, true,
/*120 x     y     z     {      |      }      ~      del      127*/
      true, true, true, true, true, true, true, false
  };

  return (c >= 0) ? isvalue[c] : false;
}
}

bool HttpParser::isFinished() const {
  return (f_.err != kHPEFine) && (f_.err != kHPENotAvailable) && (f_.err != kHPEExceptedContent);
}

bool HttpParser::parseable() {
  // A complete request's should greater than 18 bytes
  // GET / HTTP/1.0
  // method : 3 bytes at least
  // spaces : 2 bytes
  // uri    : 1 bytes at least
  // version: 8 bytes at least
  // CRLF   : 4 bytes at least

  switch (f_.state) {
  case kHPSBegin:
  case kHPSMetOptions:
  case kHPSMetGet:
  case kHPSMetPost:
  case kHPSMetHead:
    return stream_->readable() >= 16;
  case kHPSMetAlmostDone:
  case kHPSMetDone:
  case kHPSUriStart:
    return stream_->readable() >= 12;
  case kHPSUri:
  case kHPSUriEnd:
  case kHPSVerHttpSlash:
    return stream_->readable() >= 10;
  case kHPSVerMajor:
    return stream_->readable() >= 9;
  case kHPSVerDot:
  case kHPSHeader:
  case kHPSColon:
  case kHPSValue:
    return stream_->readable() >= 3;
  case kHPSVerMinor:
  case kHPSVerEnd:
    return stream_->readable() >= 2;
  case kHPSCR:
  case kHPSCRLF:
  case kHPSCRLFCR:
    return stream_->readable() >= 1;
  case kHPSDied:
    if (f_.err == kHPEExceptedContent) return stream_->readable() >= f_.content_length;
    return false;
  }

  return false;
}

bool HttpParser::parseOnce() {
  static constexpr char CR = '\r';
  static constexpr char LF = '\n';
  static constexpr size_t kBufferSize = 8;
  static const char *length_key = "Content-Length";
  static const char *connection_key = "Connection";
  static const char *keep_alive_value = "Keep-Alive";
  static const char *close_value = "Close";

  char buffer[kBufferSize];

#define stmap(cond, nextstate) if (cond) { f_.state = nextstate; break; }
#define stmapr(cond, nextstate, sth) if (cond) { f_.state = nextstate; { sth; } break; }
#define stmaprc(cond, nextstate, sth) if (cond) { f_.state = nextstate; { sth; } continue; }
#define reject(e) f_.state = kHPSDied; f_.err = e; break;
#define strequ(str, len) memcmp(buffer, str, len) == 0

  while (f_.state != kHPSDied && stream_->readable() != 0) {
    const char c = stream_->peek();
    switch (f_.state) {
    case kHPSBegin:
      stmap(c == ' ', kHPSBegin);
      stmap(c == 'O', kHPSMetOptions);
      stmap(c == 'G', kHPSMetGet);
      stmap(c == 'H', kHPSMetHead);
      stmap(c == 'P', kHPSMetPost);
      reject(kHPEInvalidMethod);
    case kHPSMetOptions:
      stream_->read(buffer, 6);
      stmaprc(strequ("PTIONS", 6), kHPSMetAlmostDone, f_.method = kHMOptions);
      reject(kHPEInvalidMethod)
    case kHPSMetGet:
      stream_->read(buffer, 2);
      stmaprc(strequ("ET", 2), kHPSMetAlmostDone, f_.method = kHMGet);
      reject(kHPEInvalidMethod);
    case kHPSMetHead:
      stream_->read(buffer, 3);
      stmaprc(strequ("EAD", 3), kHPSMetAlmostDone, f_.method = kHMHead);
      reject(kHPEInvalidMethod);
    case kHPSMetPost:
      stream_->read(buffer, 3);
      stmaprc(strequ("OST", 3), kHPSMetAlmostDone, f_.method = kHMPost);
      reject(kHPEInvalidMethod);
    case kHPSMetAlmostDone:
      stmap(c == ' ', kHPSMetDone);
      reject(kHPEInvalidMethod);
    case kHPSMetDone:
      stmap(c == ' ', kHPSMetDone);
      stmapr(isuri(c), kHPSUriStart, f_.uri_starts = stream_->readptr());
      reject(kHPEInvalidUri);
    case kHPSUriStart:
      stmap(c == ' ', kHPSUriEnd);
      stmapr(isuri(c), kHPSUri, ++f_.uri_length);
      reject(kHPEInvalidUri);
    case kHPSUri:
      stmapr(isuri(c), kHPSUri, ++f_.uri_length);
      stmap(c == ' ', kHPSUriEnd);
      reject(kHPEInvalidUri)
    case kHPSUriEnd:
      stream_->read(buffer, 5);
      stmaprc(strequ("HTTP/", 5), kHPSVerHttpSlash, ;);
      reject(kHPEInvalidVersion);
    case kHPSVerHttpSlash:
      stmapr(isdigit(c), kHPSVerMajor, f_.ver_major = c - '0');
      reject(kHPEInvalidVersion);
    case kHPSVerMajor:
      stmapr(isdigit(c), kHPSVerMajor, (f_.ver_major *= 10, f_.ver_major += c - '0'));
      stmap(c == '.', kHPSVerDot);
      reject(kHPEInvalidVersion);
    case kHPSVerDot:
      stmapr(isdigit(c), kHPSVerMinor, f_.ver_minor = c - '0');
      stmap(c == CR, kHPSVerEnd);
      reject(kHPEInvalidVersion);
    case kHPSVerMinor:
      stmapr(isdigit(c), kHPSVerMinor, (f_.ver_minor *= 10, f_.ver_minor += c - '0'));
      stmap(c == CR, kHPSVerEnd);
      reject(kHPEInvalidVersion);
    case kHPSVerEnd:
      stmapr((f_.ver_major != 1 && (f_.ver_minor != 0 && f_.ver_minor != 1)),
             kHPSDied,
             f_.err = kHPEUnsupportedVersion);
      stmap(c == LF, kHPSCRLF);
      reject(kHPEInvalidVersion);
    case kHPSCR: {
      StringView header{f_.header_starts, f_.header_length},
          value{f_.value_starts, f_.value_length};

      request_->set_field(header, value);
      f_.header_starts = f_.value_starts = nullptr;
      f_.header_length = f_.value_length = 1;

      if (header.equalsWithoutCase(length_key)) {
        for (int i = 0; i < value.readable(); i++) {
          if (isdigit(value.readptr()[i])) {
            f_.content_length *= 10;
            f_.content_length += value.readptr()[i] - '0';
          } else {
            f_.content_length = -1;
          }
        }
        if (f_.content_length == -1) {
          reject(kHPEInvalidContentLength);
        }
      }

      if (header.equalsWithoutCase(connection_key)) {
        f_.keep_alive_set = true;
        if (value.equalsWithoutCase(keep_alive_value)) {
          f_.keep_alive = true;
        } else if (value.equalsWithoutCase(close_value)) {
          f_.keep_alive = false;
        } else {
          reject(kHPEInvalidHeader);
        }
      }
      stmap(c == LF, kHPSCRLF);
      reject(kHPECRLF);
    }
    case kHPSCRLF:
      stmap(c == CR, kHPSCRLFCR);
      stmapr(isheader(c), kHPSHeader, f_.header_starts = stream_->readptr());
      reject(kHPEInvalidHeader);
    case kHPSCRLFCR:
      stmapr(c == LF, kHPSDied, f_.err = kHPEExceptedContent);
      reject(kHPECRLF);
    case kHPSHeader:
      stmapr(isheader(c), kHPSHeader, ++f_.header_length);
      stmap(c == ':', kHPSColon);
      reject(kHPEUnrecognizedChar);
    case kHPSColon:
      stmap(c == ' ', kHPSColon);
      stmapr(isvalue(c), kHPSValue, f_.value_starts = stream_->readptr());
      reject(kHPEUnrecognizedChar);
    case kHPSValue:
      stmapr(isvalue(c), kHPSValue, ++f_.value_length);
      stmap(c == CR, kHPSCR);
      reject(kHPEUnrecognizedChar);
    default:
    reject(kHPEUnexceptedEnd);
    }
    stream_->read(1);
  }

#undef stmap
#undef stmapr
#undef stmaprc
#undef reject
#undef strequ

  return f_.state == kHPSDied;
}

void HttpParser::setParseResult() {
  if (f_.content_length >= ConfigProvider::get().serverMaxContentLength()) {
    f_.err = kHPEEntityTooLarge;
    return;
  }

  switch (f_.err) {
  case kHPEExceptedContent:
    request_->set_method(f_.method);
    request_->set_uri({f_.uri_starts, f_.uri_length});
    break;
  case kHPEUnsupportedVersion:
    request_->set_code(kHCHttpVersionNotSupported);
    break;
  case kHPEInvalidMethod:
    request_->set_code(kHCNotImplemented);
    break;
  case kHPEEntityTooLarge:
    request_->set_code(kHCRequestEntityTooLarge);
    break;
  case kHPEFine:
    request_->set_code(kHCInternalServerError);
    break;
  default:
    request_->set_code(kHCBadRequest);
    break;
  }

  if (!f_.keep_alive_set) {
    f_.keep_alive = (f_.ver_minor != 0);
  }

  if (f_.keep_alive) {
    request_->set_flag(kHFKeepAlive);
  }

  request_->set_content_starts(stream_->readptr());
  request_->set_content_length(f_.content_length);
  request_->set_error(f_.err);
  request_->set_major_version(f_.ver_major);
  request_->set_minor_version(f_.ver_minor);
}

HttpParserErrors HttpParser::doParse() {
  if (f_.err != kHPEParsed) {
    if (!parseable()) {
      f_.err = kHPENotAvailable;
    } else if (f_.err == kHPEExceptedContent) {
      f_.err = kHPEParsed;
      setParseResult();
    } else if (parseOnce()) {
      setParseResult();
    }

    if (f_.err == kHPEExceptedContent && parseable()) {
      f_.err = kHPEParsed;
      setParseResult();
    }
  }

  return f_.err;
}

}