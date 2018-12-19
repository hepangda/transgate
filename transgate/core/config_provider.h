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
#include <regex>

#include "../utils/file_proxy.h"

namespace tg {

enum FcgiMode {
  kFMInet,
  kFMUnix,
};

class HostConfig {
 public:
  const std::string &host() const { return host_; }
  const FileProxy &wwwroot() const { return *wwwroot_; }
  bool isForbidden(const char *uri) const;
  FileProxy defaultFiles() const;

  void set_host(std::string host) { host_ = std::move(host); }
  void set_wwwroot(const std::string &wwwroot) { wwwroot_ = std::make_unique<FileProxy>(wwwroot.c_str()); }
  void set_default_file(std::string filename);
  void set_forbidden_regex(std::regex regex) { forbidden_regexs_.emplace_back(regex); }
 private:
  std::string host_;
  std::unique_ptr<FileProxy> wwwroot_ = nullptr;
  std::vector<std::unique_ptr<FileProxy>> default_files_;
  std::vector<std::regex> forbidden_regexs_;
};

class FcgiConfig {
 public:
  FcgiMode mode() const { return mode_; }
  const char *gateway() const { return gateway_.c_str(); }
  bool isExtends(const char *extends);
  int port() const { return port_; }

  void set_mode(FcgiMode mode) { mode_ = mode; }
  void set_gateway(std::string gateway) { gateway_ = std::move(gateway); }
  void set_port(int port) { port_ = port; }
  void set_extneds(std::string extends) { extends_.emplace_back(extends); }
 private:
  FcgiMode mode_;
  std::string gateway_;
  std::vector<std::string> extends_;
  int port_;
};

class ConfigProvider {
 public:
  int evloopEpollEvents() const { return evloop_epoll_events_; }
  int serverPort() const { return server_port_; }
  int serverMaxContentLength() const { return server_max_content_length_; }
  int serverKeepConnectionTime() const { return server_keep_connection_time_; }
  unsigned int serverInstances() const { return server_instances_; }

  static const ConfigProvider &get() {
    static ConfigProvider cp;
    return cp;
  }
 private:
  ConfigProvider() = default;

  int evloop_epoll_events_ = 100;        // Trasgate (o)
  int server_port_ = 8080;               // Transgate (o)
  int server_max_content_length_ = 256;  // HttpParser (o)
  int server_keep_connection_time_ = 20; // User
  unsigned int server_instances_ = 0;    // ConcurrencyProxy (o)

  std::vector<HostConfig> hosts_;
  std::vector<FcgiConfig> fcgis_;
};

}

#endif // TRANSGATE_CONFIG_PROVIDER_H
