#  Copyright 2018 hepangda
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

# all:
# 	clang++ transgate.cpp ../http_parser/http_parser.c -g -std=c++17 -llibgo -lpthread -lboost_thread -lboost_system
# release:
# 	clang++ transgate.cpp ../http_parser/http_parser.c -std=c++17 -llibgo -lpthread -lboost_thread -lboost_system -O2
all:
	clang++ transgate.cpp ../http_parser/http_parser.c -g -std=c++17 -lpthread
release:
	clang++ transgate.cpp ../http_parser/http_parser.c -std=c++17 -lpthread -O2
