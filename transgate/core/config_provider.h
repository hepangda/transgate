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
#include "../utils/file_reader.h"

namespace tg {

enum FcgiMode {
  kFMInet,
  kFMUnix,
};

class FcgiConfig {
 public:
  FcgiMode mode() const { return mode_; }
  const char *gateway() const { return gateway_.c_str(); }
  bool isExtends(const char *extends) const;
  int port() const { return port_; }

  void set_mode(FcgiMode mode) { mode_ = mode; }
  void set_gateway(std::string gateway) { gateway_ = std::move(gateway); }
  void set_port(int port) { port_ = port; }
  void set_extends(std::string extends) { extends_.emplace_back(extends); }
 private:
  FcgiMode mode_;
  std::string gateway_;
  std::vector<std::string> extends_;
  int port_;
};

class HostConfig {
 public:
  const std::string &host() const { return host_; }
  std::shared_ptr<FileProxy> wwwroot() const { return wwwroot_; }
  bool isForbidden(const char *uri) const;
  bool isEnabledFastcgi() const { return !fcgi_config_.empty(); }
  std::shared_ptr<FileReader> defaultFile(const char *prefix) const;
  std::shared_ptr<FcgiConfig> adaptFcgi(const char *extends) const;
  std::string wwwroot_str() const { return wwwroot_str_; }

  void set_host(std::string host) { host_ = std::move(host); }
  void set_wwwroot(const std::string &wwwroot) {
    wwwroot_str_ = wwwroot;
    wwwroot_ = std::make_shared<FileProxy>(wwwroot.c_str());
  }
  void set_default_file(std::string filename) { default_files_.emplace_back(filename); }
  void set_forbidden_regex(std::regex regex) { forbidden_regexes_.emplace_back(regex); }
  void set_fcgi_config(std::shared_ptr<FcgiConfig> fcgi) { fcgi_config_.emplace_back(fcgi); }
 private:
  std::string host_;
  std::string wwwroot_str_ = "";
  std::shared_ptr<FileProxy> wwwroot_ = nullptr;
  std::vector<std::string> default_files_;
  std::vector<std::regex> forbidden_regexes_;
  std::vector<std::shared_ptr<FcgiConfig>> fcgi_config_;
};

class ConfigProvider {
 public:
  int evloopEpollEvents() const { return evloop_epoll_events_; }
  int serverPort() const { return server_port_; }
  int serverMaxContentLength() const { return server_max_content_length_; }
  int serverKeepConnectionTime() const { return server_keep_connection_time_; }
  unsigned int serverInstances() const { return server_instances_; }
  std::shared_ptr<HostConfig> adaptHost(const char *host) const;

  static const ConfigProvider &get() {
    static ConfigProvider cp;
    return cp;
  }
 protected:
  ConfigProvider();
 private:
  int evloop_epoll_events_ = 100;        // Trasgate (o)
  int server_port_ = 8080;               // Transgate (o)
  int server_max_content_length_ = 256;  // HttpParser (o)
  int server_keep_connection_time_ = 20; // User
  unsigned int server_instances_ = 0;    // ConcurrencyProxy (o)

  std::vector<std::shared_ptr<HostConfig>> hosts_;
  bool exist_anyhost_ = false;
};

}

#endif // TRANSGATE_CONFIG_PROVIDER_H
