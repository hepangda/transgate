//
// Created by pangda on 1/4/19.
//

#include "byteorder.h"

#include <arpa/inet.h>

namespace tg {

uint16_t HostToNetwork16(short n) {
  return ::htons(static_cast<uint16_t >(n));
}

uint16_t HostToNetwork16(int n) {
  return ::htons(static_cast<uint16_t >(n));
}

short NetworkToHost16(uint16_t n) {
  return ::ntohs(n);
}

uint32_t HostToNetwork32(int n) {
  return ::htonl(static_cast<uint32_t>(n));
}

int NetworkToHost32(uint32_t n) {
  return ::ntohl(n);
}

}