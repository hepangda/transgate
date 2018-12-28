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

#include "config_provider.h"

#include <cstdio>
#include <thread>

#include "../rapidjson/document.h"
#include "../rapidjson/filereadstream.h"

namespace tg {

namespace detail {

struct CFileProxy {
  FILE *fp = nullptr;
  CFileProxy(const char *path, const char *mode) :
      fp(fopen(path, mode)) {}

  ~CFileProxy() {
    if (fp != nullptr)
      fclose(fp);
  }
};

}

bool FcgiConfig::isExtends(const char *extends) const {
  for (const auto &i: extends_) {
    if (i == extends) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<FileReader> HostConfig::defaultFile(const char *prefix) const {
  std::shared_ptr<FileProxy> directory = nullptr;

  if (strcmp(prefix, "") == 0) {
    directory = wwwroot_;
  } else {
    directory = std::make_shared<FileProxy>(*wwwroot_, prefix);
  }

  if (!directory->good())
    return nullptr;

  for (const auto &i: default_files_) {
    auto r = std::make_shared<FileReader>(*directory, i.c_str());
    if (r->good()) {
      return r;
    }
  }
  return nullptr;
}

bool HostConfig::isForbidden(const char *uri) const {
  for (const auto &i: forbidden_regexes_) {
    if (std::regex_match(uri, i)) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<FcgiConfig> HostConfig::adaptFcgi(const char *extends) const {
  for (const auto &i: fcgi_config_) {
    if (i->isExtends(extends)) {
      return i;
    }
  }
  return nullptr;
}

std::shared_ptr<HostConfig> ConfigProvider::adaptHost(const char *host) const {
  if (exist_anyhost_) {
    return hosts_.front();
  }
  for (const auto &i: hosts_) {
    if (i->host() == host) {
      return i;
    }
  }
  return nullptr;
}

ConfigProvider::ConfigProvider() {
  try {
    detail::CFileProxy fconfig{"/etc/transgate/transgate.json", "rb"};
    auto read_buffer = std::make_unique<char[]>(8192);

    rapidjson::FileReadStream fstream(fconfig.fp, read_buffer.get(), 8192);
    rapidjson::Document d;
    d.ParseStream(fstream);

    evloop_epoll_events_ = d["evloop"]["epoll_events"].GetInt();
    server_port_ = d["server"]["port"].GetInt();
    server_max_content_length_ = d["server"]["max_content_length"].GetInt();
    server_keep_connection_time_ = d["server"]["keep_connection_time"].GetInt();
    server_instances_ = d["server"]["instances"].GetUint();
    if (server_instances_ == 0) {
      server_instances_ = std::thread::hardware_concurrency();
    }

    for (const auto &i: d["sites"].GetArray()) {
      if (i["enable"].GetBool()) {
        bool is_any = strcmp(i["host"].GetString(), "<any>") == 0;
        if (is_any) {
          hosts_.clear();
        }
        auto hc = std::make_shared<HostConfig>();
        hc->set_host(i["host"].GetString());
        hc->set_wwwroot(i["wwwroot"].GetString());
        for (const auto &j: i["default_files"].GetArray()) {
          hc->set_default_file(j.GetString());
        }
        for (const auto &j: i["forbidden_regexes"].GetArray()) {
          hc->set_forbidden_regex(std::regex(j.GetString()));
        }
        for (const auto &j: i["fastcgi"].GetArray()) {
          if (j["enable"].GetBool()) {
            auto fc = std::make_shared<FcgiConfig>();
            fc->set_mode((strcmp(j["mode"].GetString(), "unix") == 0) ? kFMUnix : kFMInet);
            if (fc->mode() == kFMInet) {
              fc->set_port(j["port"].GetInt());
            }
            fc->set_gateway(j["gateway"].GetString());
            for (const auto &k: j["extends"].GetArray()) {
              fc->set_extends(k.GetString());
            }
            hc->set_fcgi_config(std::move(fc));
          }
        }
        hosts_.emplace_back(std::move(hc));
        if (is_any) {
          exist_anyhost_ = true;
          break;
        }
      }
    }
  } catch (const std::exception &ex) {
    puts("Transgate started failed, because the failure of reading config file.");
    printf("Error: %s\n", ex.what());
  }
}

}