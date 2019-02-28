/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <endian.h>
#include "sled/platform.h"

#ifdef WIN32
uint16_t htobe16(uint16_t host_16bits) { return ::htobe16(host_16bits); }
uint16_t htole16(uint16_t host_16bits) { return ::htole16(host_16bits); }
uint16_t be16toh(uint16_t big_endian_16bits) {
  return ::be16toh(big_endian_16bits);
}
uint16_t le16toh(uint16_t little_endian_16bits) {
  return ::le16toh(little_endian_16bits);
}

uint32_t htobe32(uint32_t host_32bits) { return ::htobe32(host_32bits); }
uint32_t htole32(uint32_t host_32bits) { return ::htole32(host_32bits); }
uint32_t be32toh(uint32_t big_endian_32bits) {
  return ::be32toh(big_endian_32bits);
}
uint32_t le32toh(uint32_t little_endian_32bits) {
  return ::le32toh(little_endian_32bits);
}

uint64_t htobe64(uint64_t host_64bits) { return ::htobe64(host_64bits); }
uint64_t htole64(uint64_t host_64bits) { return ::htole64(host_64bits); }
uint64_t be64toh(uint64_t big_endian_64bits) {
  return ::be64toh(big_endian_64bits);
}
uint64_t le64toh(uint64_t little_endian_64bits) {
  return ::le64toh(little_endian_64bits);
}
#else
#include <endian.h>
#endif
