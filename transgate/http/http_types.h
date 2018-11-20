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
  kHCForbidden = 404,
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

}

#endif // TRANSGATE_HTTP_TYPES_H
