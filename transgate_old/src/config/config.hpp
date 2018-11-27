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
#include <functional>
#include <memory>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../rapidjson/document.h"
#include "../rapidjson/filereadstream.h"

#include "../protocol/http_types.hpp"
#include "../utils/debug.hpp"
#define TRANSGATE_VERSION_STRING "0.1.0"

namespace tg {

class ConfigLoader;
typedef int SiteID;

namespace config {

inline unsigned int PORT;
inline int TGEPOLL_EVN;

class Sites {
  friend class tg::ConfigLoader;

 public:
  struct _Site {
    std::regex host;
    std::string wwwroot;
    std::string default_file;
    std::vector<std::regex> forbidden_pattern;
    bool allow_get = false;
    bool allow_post = false;
    bool allow_head = false;
    bool allow_options = false;
    _Site(std::regex host, std::string wwwroot, std::string default_file,
          std::vector<std::regex> forbidden_pattern, bool allow_get,
          bool allow_post, bool allow_head, bool allow_options)
        : host(host),
          wwwroot(wwwroot),
          default_file(default_file),
          forbidden_pattern(forbidden_pattern),
          allow_get(allow_get),
          allow_post(allow_post),
          allow_head(allow_head),
          allow_options(allow_options) {}
  };

 private:
  static std::vector<_Site> _cont;

 public:
  inline static SiteID matchHost(std::string host) {
    for (int i = 0; i < _cont.size(); i++) {
      if (std::regex_match(host, _cont[i].host)) {
        return SiteID(i);
      }
    }

    return SiteID(-1);
  }

  inline static std::string getFullpath(SiteID id, std::string &name) {
    if (*name.rbegin() == '/') {
      name += _cont[id].default_file;
    }
    return _cont[id].wwwroot + name;
  }

  // 1 for OK
  // 0 for Not Allowed
  // -1 for Not Implemented
  inline static int methodAllowed(SiteID id, HttpMethod method) {
    switch (method) {
      case HttpMethod::GET:
        return _cont[id].allow_get;
      case HttpMethod::POST:
        return _cont[id].allow_post;
      case HttpMethod::OPTIONS:
        return _cont[id].allow_options;
      case HttpMethod::HEAD:
        return _cont[id].allow_head;
      default:
        return -1;
    }
  }

  inline static bool isForbidden(SiteID id, std::string name) {
    for (auto &i : _cont[id].forbidden_pattern) {
      if (std::regex_match(name, i)) {
        return true;
      }
    }

    return false;
  }
};

class Http {
  friend class tg::ConfigLoader;

 private:
  static std::unordered_map<int16_t, std::string> _desc;
  static std::unordered_map<std::string, std::string> _conttype;
  static std::string _conttype_any;
  const static char *_desc_err;

  static inline bool endswith(std::string_view str, std::string_view end) {
    if (str.size() >= end.size()) {
      str.remove_prefix(str.size() - end.size());
      return str == end;
    }
    return false;
  }

 public:
  inline static const char *getStatusDesc(int16_t code) {
    try {
      return _desc.at(code).c_str();
    } catch (...) {
      return _desc_err;
    }
  }

  inline static std::string_view getContentType(std::string_view filename) {
    for (auto &i : _conttype) {
      if (endswith(filename, {i.first})) {
        return {i.second};
      }
    }
    return {_conttype_any};
  }
};

class Fastcgi {
  friend class tg::ConfigLoader;

 private:
  static std::regex reCgi;
  static std::shared_ptr<char> cgiAddr;
  static unsigned int port;

 public:
  inline static bool isCgi(std::string uri) {
    return std::regex_match(uri, reCgi);
  }

  inline static char *getAddr() { return cgiAddr.get(); }

  inline static unsigned int getPort() { return port; }
};

}  // namespace config

class ConfigLoader {
 private:
  static inline void loadHttp() {
    FILE *fp = fopen("../config/http.json", "rb");
    char *readbuf = new char[4096];

    rapidjson::FileReadStream stream(fp, readbuf, 4096);
    rapidjson::Document doc;
    doc.ParseStream(stream);

    config::Http::_conttype_any = doc["content-type-any"].GetString();
    for (auto &i : doc["content-types"].GetArray()) {
      config::Http::_conttype[i["ext"].GetString()] = i["type"].GetString();
    }
    for (auto &i : doc["http-status-string"].GetArray()) {
      config::Http::_desc[i["status"].GetInt()] = i["description"].GetString();
    }
    delete[] readbuf;
    fclose(fp);
  }

  static inline void loadGlobal() {
    FILE *fp = fopen("../config/transgate.json", "rb");
    char *readbuf = new char[4096];

    rapidjson::FileReadStream stream(fp, readbuf, 4096);
    rapidjson::Document doc;
    doc.ParseStream(stream);

    config::PORT = doc["global"]["port"].GetInt();
    config::TGEPOLL_EVN = doc["global"]["epoll_events"].GetInt();

    std::vector<std::string> sites;
    for (auto &i : doc["sites-avaiable"].GetArray()) {
      sites.emplace_back(i.GetString());
    }

    for (auto &i : sites) {
      auto &j = doc[i.c_str()];
      std::vector<std::regex> fbreg;

      for (auto &k : j["forbidden-regex"].GetArray()) {
        fbreg.emplace_back(k.GetString());
      }

      config::Sites::_cont.emplace_back(
          std::regex(j["host-regex"].GetString()), j["www-root"].GetString(),
          j["default-file"].GetString(), fbreg,
          j["allowed-methods"]["GET"].GetBool(),
          j["allowed-methods"]["POST"].GetBool(),
          j["allowed-methods"]["HEAD"].GetBool(),
          j["allowed-methods"]["OPTIONS"].GetBool());
    }

    config::Fastcgi::port = doc["fastcgi"]["tcp-port"].GetInt();
    config::Fastcgi::cgiAddr = {
        new char[doc["fastcgi"]["tcp-addr"].GetStringLength()],
        std::default_delete<char[]>()};
    strcpy(config::Fastcgi::cgiAddr.get(),
           doc["fastcgi"]["tcp-addr"].GetString());
    config::Fastcgi::reCgi = std::regex(doc["fastcgi"]["match"].GetString());

    delete[] readbuf;
    fclose(fp);
  }

 public:
  static inline void load() {
    loadHttp();
    loadGlobal();
  }
};

}  // namespace tg