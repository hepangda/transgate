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

#ifndef TRANSGATE_HTTP_TYPES_H
#define TRANSGATE_HTTP_TYPES_H

namespace tg {

enum HttpMethod {
  kHMOptions = 0,
  kHMGet,
  kHMHead,
  kHMPost,
  kHMInvalid,
};

enum HttpStatusCode {
  kHCNotDone = -1,
  kHCOk = 200,
  kHCPartialContent = 206,
  kHCMovedPermanently = 301,
  kHCFound = 302,
  kHCBadRequest = 400,
  kHCForbidden = 403,
  kHCNotFound = 404,
  kHCMethodNotAllowed = 405,
  kHCLengthRequired = 411,
  kHCRequestEntityTooLarge = 413,
  kHCInternalServerError = 500,
  kHCNotImplemented = 501,
  kHCBadGateway = 502,
  kHCGatewayTimeout = 504,
  kHCHttpVersionNotSupported = 505
};

enum HttpFlags {
  kHFNothing = 0,
  kHFKeepAlive = 1,
};

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
  kHPEOther,
};

constexpr const char *httpCodeDescription(int code) {
  switch (code) {
  case kHCOk:
    return "OK";
  case kHCPartialContent:
    return "Partial Content";
  case kHCMovedPermanently:
    return "Moved Permanently";
  case kHCFound:
    return "Found";
  case kHCBadRequest:
    return "Bad Request";
  case kHCForbidden:
    return "Forbidden";
  case kHCNotFound:
    return "Not Found";
  case kHCMethodNotAllowed:
    return "Method Not Allowed";
  case kHCLengthRequired:
    return "Length Required";
  case kHCRequestEntityTooLarge:
    return "Request Entity Too Large";
  case kHCInternalServerError:
    return "Internal Server Error";
  case kHCNotImplemented:
    return "Not Implemented";
  case kHCBadGateway:
    return "Bad Gateway";
  case kHCGatewayTimeout:
    return "Gateway Timeout";
  case kHCHttpVersionNotSupported:
    return "Http Version Not Supported";
  default:
    return "Unknown";
  }
}

}

#endif // TRANSGATE_HTTP_TYPES_H
