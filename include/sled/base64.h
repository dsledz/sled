/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace sled {

/**
 * Decode a base64 string (with padding).
 *
 * Decoded vector must match the expected size.
 */
std::vector<std::byte> base64_decode(const std::string &input,
                                     size_t expected_size);

/**
 * Decode a bas64 string (with padding).
 */
std::vector<std::byte> base64_decode(const std::string &input);

/**
 * Encode a base64 string (with padding) over [start, end).
 */
std::string base64_encode(const std::byte *begin, const std::byte *end);

/**
 * Encode a base64 string (with padding).
 */
template <typename T>
std::string base64_encode(const std::vector<T> &x) {
  auto *begin = reinterpret_cast<const std::byte *>(x.data());
  auto *end = reinterpret_cast<const std::byte *>(x.data() + x.size());
  return base64_encode(begin, end);
}

template <size_t SIZE>
std::string base64_encode(const std::array<uint8_t, SIZE> &x) {
  auto *begin = reinterpret_cast<const std::byte *>(x.data());
  auto *end = reinterpret_cast<const std::byte *>(x.data() + SIZE);
  return base64_encode(begin, end);
}

/**
 * Encode a base64 string (with padding).
 */
template <typename T, size_t SIZE>
std::string base64_encode(const std::array<T, SIZE> &x) {
  auto *begin = reinterpret_cast<const std::byte *>(x.data());
  auto *end = reinterpret_cast<const std::byte *>(x.data() + SIZE);
  return base64_encode(begin, end);
}

};  // namespace sled
