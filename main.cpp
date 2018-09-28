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

#include <vector>

#include "transgate/core/tg.h"

int main() {
//  shared_ptr<Buffer> buf = make_shared<StringView>(
//        "GET /home/ptr HTTP/1.1\r\nContent-Length:300\r\nConnection: Close\r\nHost: Fuck.com\r\n\r\n"
//      );
//  shared_ptr<HttpRequest> req = make_shared<HttpRequest>();
//  HttpParser parser(buf, req);
//  parser.doParse();
//
//  cout << req->uri() << endl;
//  for (auto i: req->fields()) {
//    cout << i.first << ": " << i.second << endl;
//  }
//
//  cout << req->method() << endl;

  std::vector<std::thread> threads;
  threads.reserve(std::thread::hardware_concurrency());

  for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
    threads.emplace_back([] {
      tg::Transgate app;
      app.run();
    });
  }

  for (auto &i: threads) {
    i.join();
  }

  return 0;
}