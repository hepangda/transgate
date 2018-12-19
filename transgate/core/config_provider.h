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

#ifndef TRANSGATE_CONFIG_PROVIDER_H
#define TRANSGATE_CONFIG_PROVIDER_H

#include <string>
#include <vector>

namespace tg {

class SiteConfig {
 public:
 private:
};

class ConfigProvider {
 public:
  int evloopEpollEvents() const { return evloop_epoll_events_; }
  int serverPort() const { return server_port_; }
  int serverMaxContentLength() const { return server_max_content_length_; }
  int serverKeepConnectionTime() const { return server_keep_connection_time_; }
  int serverInstances() const { return server_instances_; }

//  const char *hostDefaultFiles() const { return host_default_files_; }
//  bool fcgiExtends() const { return ; }
  int fcgiMode() const { return fastcgi_mode_; }
  int fcgiPort() const { return fastcgi_port_; }
  const char *fcgiGateway() const { return fastcgi_gateway_; }
 private:
  int evloop_epoll_events_; // Trasgate
  int server_port_ = 80; // Transgate
  int server_max_content_length_ = 256; //HttpParser
  int server_keep_connection_time_ = 20; // User
  int server_instances_ = -1; // ConcurrencyProxy

  std::vector<std::string> host_default_files_ = { "index.html" };
  std::vector<std::string> fastcgi_extends_;

  // FastCGI Mode
  // 1 AF_INET
  // 2 AF_UNIX
  int fastcgi_mode_ = 1;
  int fastcgi_port_ = 9000;
  const char *fastcgi_gateway_ = "localhost";
};

}

#endif // TRANSGATE_CONFIG_PROVIDER_H
