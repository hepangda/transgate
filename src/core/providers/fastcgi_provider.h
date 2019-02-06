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

#ifndef TRANSGATE_FASTCGI_PROVIDER_H
#define TRANSGATE_FASTCGI_PROVIDER_H

#include "provider.h"
#include "../../net/tcp_client.h"
namespace tg {

enum FastcgiStage : int {
  kFSBegin = 0,
  kFSConnected,
  kFSSent,
  kFSRecving,
  kFSParsing,
  kFSForwarding,
  kFSDone,
};

class FastcgiProvider : public Provider {
 public:
  FastcgiProvider(const std::shared_ptr<HttpRequest> &request,
                  const std::shared_ptr<WriteLoop> &write_loop,
                  const std::shared_ptr<HostConfig> &host,
                  std::shared_ptr<FcgiConfig> fcgi) :
      Provider(request, write_loop, host), fcgi_config_(std::move(fcgi)) {}
  void provide() final;
  FastcgiStage stage() const { return stage_; }
  std::shared_ptr<TcpClient> gateway() const { return gateway_; }
  std::shared_ptr<FcgiConfig> config() const { return fcgi_config_; }
  void set_gateway(std::shared_ptr<TcpClient> gateway) { gateway_ = std::move(gateway); }
  void set_stage(FastcgiStage stage) { stage_ = stage; }
  std::shared_ptr<CharBuffer> buffer() { return buffer_; }
  bool parse_end() const { return end; }
  bool forward_end() const { return out_->readable() == 0; }
  void makeRequest();
  void consume();
  void parse();
  void doForward() { write_loop_->doAll(); }
  bool isHeaderEnds();
 private:
  std::shared_ptr<FcgiConfig> fcgi_config_;
  std::shared_ptr<TcpClient> gateway_ = nullptr;
  std::shared_ptr<CharBuffer> buffer_ = std::make_shared<CharBuffer>(81920);
  std::shared_ptr<CharBuffer> out_ = std::make_shared<CharBuffer>(81920);
  std::unordered_map<std::string, std::string> fields_;

  bool isStderr = false;
  bool provided = false;
  bool end = false;
  HttpStatusCode code_ = kHCOk;
  FastcgiStage stage_ = kFSBegin;
};

}

#endif // TRANSGATE_FASTCGI_PROVIDER_H
