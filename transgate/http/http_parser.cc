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

#include "http_types.h"
#include "http_request.h"
#include "http_parser.h"

namespace tg {

namespace {
inline bool isheader(char c) {
  return isalnum(c) || c == '-' || c == '_';
}
}

HttpParser::Errors HttpParser::doParse() {
  static constexpr char CR = '\r';
  static constexpr char LF = '\n';
  static const StringView length_key("Content-Length");
  static const StringView connection_key("Connection");
  static const StringView keep_alive_value("Keep-Alive");
  static const StringView close_value("Close");
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

  enum {
    kBegin,

    kMetOptions,
    kMetHead,
    kMetGet,
    kMetPost,
    kMetAlmostDone,
    kMetDone,

    kUriStart,
    kUri,
    kUriEnd,

    kVerH,
    kVerHt,
    kVerHtt,
    kVerHttp,
    kVerHttpSlash,
    kVerMajor,
    kVerDot,
    kVerMinor,
    kVerEnd,

    kCR,
    kCRLF,
    kCRLFCR,
    kCRLFCRLF,

    kHeader,
    kColon,
    kValue,
    kDied,
  } state = kBegin;

  Errors err = kEFine;

  if (stream_->readable() < 8) {
    state = kDied;
    err = kELowBufferLength;
  }

  HttpMethod method = kHMInvalid;
  const char *uri_starts = nullptr;
  int uri_length = 1;
  int ver_major = 0, ver_minor = 0;
  const char *header_starts = nullptr, *value_starts = nullptr;
  int header_length = 1, value_length = 1;
  int content_length = 0;
  bool keep_alive = true, keep_alive_set = false;

#define StateMap(cond, next_state) if (cond) \
                                    {  state = next_state; break; }
#define StateMapR(cond, next_state, something) if (cond) \
                                               { state = next_state; { something; } break; }
#define Reject(e) state = kDied; err = e; break;

  const char *p = stream_->rptr();
  for (; state != kDied && stream_->readable() != 0; stream_->doRead(1), p++) {
    const char c = *p;

    switch (state) {
    case kBegin:
      StateMap(c == ' ', kBegin);
      StateMap(c == 'O', kMetOptions);
      StateMap(c == 'G', kMetGet);
      StateMap(c == 'H', kMetHead);
      StateMap(c == 'P', kMetPost);
      Reject(kEInvalidMethod);
    case kMetOptions: {
      const char c2 = *++p, c3 = *++p, c4 = *++p, c5 = *++p, c6 = *++p;
      stream_->doRead(5);
      StateMapR(c == 'P' && c2 == 'T' && c3 == 'I' && c4 == 'O' && c5 == 'N' && c6 == 'S',
                kMetAlmostDone,
                method = kHMOptions);
      Reject(kEInvalidMethod)
    }
    case kMetGet: {
      const char c2 = *++p;
      stream_->doRead(1);
      StateMapR(c == 'E' && c2 == 'T', kMetAlmostDone, method = kHMGet);
      Reject(kEInvalidMethod);
    }
    case kMetHead: {
      const char c2 = *++p, c3 = *++p;
      stream_->doRead(2);
      StateMapR(c == 'E' && c2 == 'A' && c3 == 'D', kMetAlmostDone, method = kHMHead);
      Reject(kEInvalidMethod);
    }
    case kMetPost: {
      const char c2 = *++p, c3 = *++p;
      stream_->doRead(2);
      StateMapR(c == 'O' && c2 == 'S' && c3 == 'T', kMetAlmostDone, method = kHMPost);
      Reject(kEInvalidMethod);
    }
    case kMetAlmostDone:
      StateMap(c == ' ', kMetDone);
      Reject(kEInvalidMethod);
    case kMetDone:
      StateMap(c == ' ', kMetDone);
      StateMapR(isuri[c], kUriStart, uri_starts = p);
      Reject(kEInvalidUri);
    case kUriStart:
      StateMap(c == ' ', kUriEnd);
      StateMapR(isuri[c], kUri, ++uri_length);
      Reject(kEInvalidUri);
    case kUri:
      StateMapR(isuri[c], kUri, ++uri_length);
      StateMap(c == ' ', kUriEnd);
      Reject(kEInvalidUri)
    case kUriEnd:
      StateMap(c == 'H', kVerH);
      Reject(kEInvalidVersion);
    case kVerH:
      StateMap(c == 'T', kVerHt);
      Reject(kEInvalidVersion);
    case kVerHt:
      StateMap(c == 'T', kVerHtt);
      Reject(kEInvalidVersion);
    case kVerHtt:
      StateMap(c == 'P', kVerHttp);
      Reject(kEInvalidVersion);
    case kVerHttp:
      StateMap(c == '/', kVerHttpSlash);
      Reject(kEInvalidVersion);
    case kVerHttpSlash:
      StateMapR(isdigit(c), kVerMajor, ver_major = c - '0');
      Reject(kEInvalidVersion);
    case kVerMajor:
      StateMapR(isdigit(c), kVerMajor, (ver_major *= 10, ver_major += c - '0'));
      StateMap(c == '.', kVerDot);
      Reject(kEInvalidVersion);
    case kVerDot:
      StateMapR(isdigit(c), kVerMinor, ver_minor = c - '0');
      StateMap(c == CR, kVerEnd);
      Reject(kEInvalidVersion);
    case kVerMinor:
      StateMapR(isdigit(c), kVerMinor, (ver_minor *= 10, ver_minor += c - '0'));
      StateMap(c == CR, kVerEnd);
      Reject(kEInvalidVersion);
    case kVerEnd:
      StateMapR((ver_major != 1 && (ver_minor != 0 && ver_minor != 1)), kDied, err = kEUnsupportedVersion);
      StateMap(c == LF, kCRLF);
      Reject(kEInvalidVersion);
    case kCR: {
      StringView header{header_starts, header_length},
          value{value_starts, value_length};

      request_->set_field(header, value);
      header_starts = value_starts = nullptr;
      header_length = value_length = 1;

      if (header.equalsWithoutCase(length_key)) {
        for (int i = 0; i < value.readable(); i++) {
          if (isdigit(value.rptr()[i])) {
            content_length *= 10;
            content_length += value.rptr()[i] - '0';
          } else {
            content_length = -1;
          }
        }
        if (content_length == -1) {
          Reject(kEInvalidContentLength);
        }
      }

      if (header.equalsWithoutCase(connection_key)) {
        keep_alive_set = true;
        if (value.equalsWithoutCase(keep_alive_value)) {
          keep_alive = true;
        } else if (value.equalsWithoutCase(close_value)) {
          keep_alive = false;
        } else {
          Reject(kEInvalidHeader);
        }
      }
      StateMap(c == LF, kCRLF);
      Reject(kECRLF);
    }
    case kCRLF:
      StateMap(c == CR, kCRLFCR);
      StateMapR(isheader(c), kHeader, header_starts = p);
      Reject(kEInvalidHeader);
    case kCRLFCR:
      StateMap(c == LF, kCRLFCRLF);
      Reject(kECRLF);
    case kCRLFCRLF:
      Reject(kEFine);
    case kHeader:
      StateMapR(isheader(c), kHeader, ++header_length);
      StateMap(c == ':', kColon);
      Reject(kEUnrecognizedChar);
    case kColon:
      StateMap(c == ' ', kColon);
      StateMapR(isuri[c], kValue, value_starts = p);
      Reject(kEUnrecognizedChar);
    case kValue:
      StateMapR(isuri[c], kValue, ++value_length);
      StateMap(c == CR, kCR);
    default:
      Reject(kEUnexceptedEnd);
    }
  }

#undef StateMap
#undef StateMap
#undef StateMapR
#undef Reject

  if (stream_->readable() < content_length) {
    err = kEEntityTooLarge;
  }

  switch (err) {

  case kEFine:
    request_->set_method(method);
    request_->set_uri({uri_starts, uri_length});
    break;
  case kEUnsupportedVersion:
    request_->set_code(kHCHttpVersionNotSupported);
    break;
  case kEInvalidMethod:
    request_->set_code(kHCNotImplemented);
    break;
  case kEEntityTooLarge:
    request_->set_code(kHCRequestEntityTooLarge);
    break;
  default:
    request_->set_code(kHCBadRequest);
    break;
  }

  if (!keep_alive_set) {
    keep_alive = (ver_major != 0);
  }

  if (keep_alive) {
    request_->set_flag(kHFKeepAlive);
  }

  request_->set_content_starts(p);
  request_->set_content_length(content_length);

  return err;
}

}