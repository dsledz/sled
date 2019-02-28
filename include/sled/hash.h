/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"

namespace sled {
/**
 * SHA1 hash value.
 *
 * Only use with legacy code.
 */
struct sha1 {
  std::array<std::byte, 20> v;

  operator bool() const {
    static constexpr uint8_t zero_sha1[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return memcmp(v.data(), zero_sha1, sizeof(zero_sha1)) != 0;
  }

  friend std::ostream &operator<<(std::ostream &os, sha1 const &h) {
    os << "NYI" << std::endl;
    return os;
  }
};
}  // namespace sled
