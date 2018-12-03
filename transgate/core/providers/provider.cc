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

#include "provider.h"

#include <ctime>

namespace tg {

namespace detail {

constexpr const char *getDescription(int code) {
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

constexpr const char *getVersionString() { return "1.0"; }
}

void Provider::writeHead(int ver_major, int ver_minor, HttpStatusCode code) {
  write_loop_->swrite("HTTP/%d.%d %d %s\r\n", ver_major, ver_minor, code, detail::getDescription(code));
  write_loop_->swrite("Server: Transgate %s\r\n", detail::getVersionString());
}

void Provider::writeItem(const char *key, const char *value) {
  write_loop_->swrite("%s: %s\r\n", key, value);
}

void Provider::writeItemDate() {
  time_t t = time(nullptr);
  char tmbuf[64];
  strftime(tmbuf, 63, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", gmtime(&t));
  write_loop_->write(tmbuf);
}

void Provider::writeItemConnection(bool keep_alive) {
  writeItem("Connection", keep_alive ? "keep-alive" : "close");
}

}