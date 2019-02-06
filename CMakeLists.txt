cmake_minimum_required(VERSION 3.0)
project(transgate)

set(CMAKE_CXX_STANDARD 14)
link_libraries(pthread)
add_definitions("-O3")

aux_source_directory(src/base SOURCE_BASE)
aux_source_directory(src/core SOURCE_CORE)
aux_source_directory(src/core/providers SOURCE_PROVIDERS)
aux_source_directory(src/net SOURCE_NET)
aux_source_directory(src/http SOURCE_HTTP)
aux_source_directory(src/utils SOURCE_UTILS)
aux_source_directory(src/fastcgi SOURCE_FCGI)

add_executable(transgate
        src/transgate.cc
        ${SOURCE_BASE}
        ${SOURCE_CORE}
        ${SOURCE_NET}
        ${SOURCE_HTTP}
        ${SOURCE_UTILS}
        ${SOURCE_PROVIDERS}
        ${SOURCE_FCGI}
        )
