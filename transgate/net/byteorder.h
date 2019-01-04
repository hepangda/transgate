//
// Created by pangda on 1/4/19.
//

#ifndef TRANSGATE_BYTEORDER_H
#define TRANSGATE_BYTEORDER_H

#include <cstdint>

namespace tg {

uint16_t HostToNetwork16(short n);
uint16_t HostToNetwork16(int n);
short NetworkToHost16(uint16_t n);
uint32_t HostToNetwork32(int n);
int NetworkToHost32(uint32_t n);

}

#endif // TRANSGATE_BYTEORDER_H
