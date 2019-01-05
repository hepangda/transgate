cmake_minimum_required(VERSION 3.11)
project(transgate)

set(CMAKE_CXX_STANDARD 14)
link_libraries(pthread)
add_definitions("-O3")
aux_source_directory(./transgate/base SOURCE_BASE)
aux_source_directory(./transgate/core SOURCE_CORE)
aux_source_directory(./transgate/core/providers SOURCE_PROVIDERS)
aux_source_directory(./transgate/net SOURCE_NET)
aux_source_directory(./transgate/http SOURCE_HTTP)
aux_source_directory(./transgate/utils SOURCE_UTILS)
aux_source_directory(./transgate/fastcgi SOURCE_FCGI)
add_executable(transgate
        main.cc
        ${SOURCE_BASE}
        ${SOURCE_CORE}
        ${SOURCE_NET}
        ${SOURCE_HTTP}
        ${SOURCE_UTILS}
        ${SOURCE_PROVIDERS}
        ${SOURCE_FCGI}
        )