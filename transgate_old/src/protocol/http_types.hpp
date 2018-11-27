/****************************************************************************
 * Copyright 2018 hepangda                                                  *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *     http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/
#pragma once
#include <cstdint>

namespace tg {

// enum HttpMethod {
//   kHttpMethodDelete = 0,
//   kHttpMethodGet,
//   kHttpMethodHead,
//   kHttpMethodPost,
//   kHttpMethodPut,
//   kHttpMethodConnect,
//   kHttpMethodOptions,
//   kHttpMethodTrace,
// }

enum class HttpMethod : int8_t {
  DELETE = 0,
  GET = 1,
  HEAD = 2,
  POST = 3,
  PUT = 4,
  CONNECT = 5,
  OPTIONS = 6,
  TRACE = 7
};

enum class HttpRequestStatus : int8_t {
  OK = 0,
  BAD_REQUEST = 1,
  HTTP_VERSION_NOT_SUPPORTED = 2,
  KEEP_ALIVE = 3,
  NOT_FOUND = 4,
  METHOD_NOT_ALLOWED = 5,
  NOT_IMPLEMENTED = 6,
  FORBIDDEN,
};

enum class HttpEntityType : int8_t { ByteStream, File };

// enum HttpStatus {
//   kHttpStatusContinue = 100,
//   kHttpStatusSwitchingProtocols = 101,
//   kHttpStatusOK = 200,
//   kHttpStatusCreated = 201,
//   kHttpStatusAccepted = 202,
//   kHttpStatusNonAuthoritativeInformation = 203,
//   kHttpStatusNoContent = 204,
//   kHttpStatusResetContent = 205,
//   kHttpStatusPartialContent = 206,
//   kHttpStatusMultipleChoices = 300,
//   kHttpStatusMovedPermanently = 301,
//   kHttpStatusFound = 302,
//   kHttpStatusSeeOther = 303,
//   kHttpStatusNotModified = 304,
//   kHttpStatusUseProxy = 305,
//   // 306 Not used
//   kHttpStatusTemporaryRedirect = 307,
//   kHttpStatusBadRequest = 400,
//   kHttpStatusUnauthorized = 401,
//   kHttpStatusForbidden = 403,
//   kHttpStatusNotFound = 404,
//   kHttpStatusMethodNotAllowed = 405,
//   kHttpStatusNotAcceptable = 406,
// }

enum class HttpStatusCode : int16_t {
  // 1xx - Informational
  CONTINUE = 100,
  SWITCHING_PROTOCOLS = 101,

  // 2xx - Successful
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NON_AUTHORITATIVE_INFORMATION = 203,
  NO_CONTENT = 204,
  RESET_CONTENT = 205,
  PARTIAL_CONTENT = 206,

  // 3xx - Redirection
  MULTIPLE_CHOICES = 300,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  USE_PROXY = 305,
  TEMPORARY_REDIRECT = 307,

  // 4xx - Client Error
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  PROXY_AUTHENTICATION_REQUIRED = 407,
  REQUEST_TIMEOUT = 408,
  CONFLICT = 409,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  PRECONDITION_FAILED = 412,
  REQUEST_ENTITY_TOO_LARGE = 413,
  REQUEST_URI_TOO_LONG = 414,
  UNSUPPORTED_MEDIA_TYPE = 415,
  REQUEST_RANGE_NOT_SATISFIABLE = 416,
  EXPECTATION_FAILED = 417,

  // 5xx - Server Error
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
  GATEWAY_TIMEOUT = 504,
  HTTP_VERSION_NOT_SUPPORTED = 505,
};

}  // namespace tg