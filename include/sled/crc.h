/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"
#include "sled/strong_int.h"

#include <array>
#include <type_traits>
#include <vector>

namespace sled {

struct crc32_tag {};
using crc32c = StrongInteger<uint32_t, crc32_tag>;

/**
 * Compute the CRC32C value for the vector
 */
template <typename T,
          class = typename std::enable_if_t<std::is_integral<T>::value>>
static inline crc32c calculate_crc32c(std::vector<T> const &data,
                                      crc32c initial = crc32c{0}) {
  auto crc{initial.get()};
  for (auto &b : data) {
    if constexpr (sizeof(T) == 1) {
      crc = _mm_crc32_u8(crc, static_cast<uint8_t>(b));
    } else if constexpr (sizeof(T) == 2) {
      crc = _mm_crc32_u16(crc, static_cast<uint16_t>(b));
    } else if constexpr (sizeof(T) == 4) {
      crc = _mm_crc32_u32(crc, static_cast<uint32_t>(b));
    } else if constexpr (sizeof(T) == 8) {
      crc = static_cast<int32_t>(_mm_crc32_u64(crc, static_cast<uint64_t>(b)));
    } else {
      throw std::invalid_argument("unsupported underlying type");
    }
  }
  return crc32c{crc};
}

/**
 * Compute the CRC32C value for the array
 */
template <typename T, size_t SIZE,
          class = typename std::enable_if_t<std::is_integral<T>::value>>
static inline crc32c calculate_crc32c(std::array<T, SIZE> const &data,
                                      crc32c initial = crc32c{0}) {
  auto crc{initial.get()};
  for (auto &b : data) {
    if constexpr (sizeof(T) == 1) {
      crc = _mm_crc32_u8(crc, static_cast<uint8_t>(b));
    } else if constexpr (sizeof(T) == 2) {
      crc = _mm_crc32_u16(crc, static_cast<uint16_t>(b));
    } else if constexpr (sizeof(T) == 4) {
      crc = _mm_crc32_u32(crc, static_cast<uint32_t>(b));
    } else if constexpr (sizeof(T) == 8) {
      crc = static_cast<int32_t>(_mm_crc32_u64(crc, static_cast<uint64_t>(b)));
    } else {
      throw std::invalid_argument("unsupported underlying type");
    }
  }
  return crc32c{crc};
}

static inline crc32c calculate_crc32c(uint8_t const *begin, uint8_t const *end,
                                      crc32c initial = crc32c{0}) {
  // XXX: _u8 is int (int, char), _u64 is int64_t (int64_t, int64_t) making
  // conversions strange.
  auto crc{initial.get()};
  while ((reinterpret_cast<intptr_t>(begin) % 8) && begin < end) {
    crc = _mm_crc32_u8(crc, *begin);
    begin++;
  }
  while (begin < end - 8) {
    crc = static_cast<int32_t>(
        _mm_crc32_u64(crc, *reinterpret_cast<const uint64_t *>(begin)));
    begin += 8;
  }
  while (begin < end) {
    crc = _mm_crc32_u8(crc, *begin);
    begin++;
  }
  return crc32c{crc};
}

};  // namespace sled
