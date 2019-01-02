/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/base64.h"
#include "sled/platform.h"

namespace sled {

static constexpr char base64_encode_lookup[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static_assert(sizeof(base64_encode_lookup) == 65);

static constexpr inline char base64_encode_byte(std::byte b) {
  int v = std::to_integer<int>(b);
  debug_assert(v >= 0 && v <= 63);
  return base64_encode_lookup[v];
}

static constexpr std::array<uint8_t, 128> base64_decode_lookup{
    /* 0x00 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x08 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x10 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x18 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x20 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x28 */ 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    /* 0x30 */ 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
    /* 0x38 */ 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x40 */ 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    /* 0x48 */ 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    /* 0x50 */ 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    /* 0x58 */ 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0x60 */ 0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    /* 0x68 */ 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    /* 0x70 */ 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
    /* 0x78 */ 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static constexpr inline std::byte base64_decode_char(int c) {
  if (c < 0 || base64_decode_lookup[c] == 0xFF) {
    throw std::invalid_argument("Invalid character during base64_decode");
  }
  return std::byte{base64_decode_lookup[c]};
}

std::vector<std::byte> base64_decode(const std::string &input) {
  std::vector<std::byte> result;
  size_t input_size = input.size();

  /* The base64 encoding of zero bytes is an empty string. */
  if (input_size == 0) {
    return result;
  }

  /*
   * A base64 string is always a multiple of 4 characters due to padding.
   */
  if (input_size % 4 != 0) {
    throw std::invalid_argument("invalid length");
  }

  /* result_size will be adjusted later for padding. */
  size_t result_size = input_size / 4 * 3;
  result.resize(result_size);

  size_t j = 0;
  size_t i = 0;
  for (; i < input_size - 4; i += 4, j += 3) {
    auto e0{base64_decode_char(input[i + 0])};
    auto e1{base64_decode_char(input[i + 1])};
    auto e2{base64_decode_char(input[i + 2])};
    auto e3{base64_decode_char(input[i + 3])};
    result[j + 0] =
        ((e0 & std::byte{0x3F}) << 2) | ((e1 & std::byte{0x30}) >> 4);
    result[j + 1] =
        ((e1 & std::byte{0x0F}) << 4) | ((e2 & std::byte{0x3C}) >> 2);
    result[j + 2] =
        ((e2 & std::byte{0x03}) << 6) | ((e3 & std::byte{0x3F}) >> 0);
  }

  if (input[i + 2] == '=' && input[i + 3] == '=') {
    auto e0{base64_decode_char(input[i + 0])};
    auto e1{base64_decode_char(input[i + 1])};
    result[j + 0] =
        ((e0 & std::byte{0x3F}) << 2) | ((e1 & std::byte{0x30}) >> 4);
    result.resize(j + 1);
  } else if (input[i + 3] == '=') {
    auto e0{base64_decode_char(input[i + 0])};
    auto e1{base64_decode_char(input[i + 1])};
    auto e2{base64_decode_char(input[i + 2])};
    result[j + 0] =
        ((e0 & std::byte{0x3F}) << 2) | ((e1 & std::byte{0x30}) >> 4);
    result[j + 1] =
        ((e1 & std::byte{0x0F}) << 4) | ((e2 & std::byte{0x3C}) >> 2);
    result.resize(j + 2);
  } else {
    auto e0{base64_decode_char(input[i + 0])};
    auto e1{base64_decode_char(input[i + 1])};
    auto e2{base64_decode_char(input[i + 2])};
    auto e3{base64_decode_char(input[i + 3])};
    result[j + 0] =
        ((e0 & std::byte{0x3F}) << 2) | ((e1 & std::byte{0x30}) >> 4);
    result[j + 1] =
        ((e1 & std::byte{0x0F}) << 4) | ((e2 & std::byte{0x3C}) >> 2);
    result[j + 2] =
        ((e2 & std::byte{0x03}) << 6) | ((e3 & std::byte{0x3F}) >> 0);
    assert(result.size() == j + 3);
  }
  return result;
}

std::vector<std::byte> base64_decode(const std::string &input,
                                     size_t expected_size) {
  auto result = base64_decode(input);

  if (result.size() != expected_size) {
    throw std::invalid_argument("Wrong size during base64 decoding");
  }

  return result;
}

std::string base64_encode(const std::byte *begin, const std::byte *end) {
  std::string result;
  std::ptrdiff_t input_size = end - begin;
  const std::byte *input = begin;

  if (input_size == 0) {
    /* An empty vector is encoded as "" */
    return result;
  }

  /* Result always include padding, so round up */
  std::size_t result_size = (input_size + 2) / 3 * 4;
  result.resize(result_size);

  std::ptrdiff_t i{0};
  std::ptrdiff_t j{0};
  for (; i < input_size - 3; i += 3, j += 4) {
    auto e0{((input[i + 0] & std::byte{0xFC}) >> 2)};
    auto e1{((input[i + 0] & std::byte{0x03}) << 4) |
            ((input[i + 1] & std::byte{0xF0}) >> 4)};
    auto e2{((input[i + 1] & std::byte{0x0F}) << 2) |
            ((input[i + 2] & std::byte{0xC0}) >> 6)};
    auto e3{((input[i + 2] & std::byte{0x3F}) >> 0)};
    result[j + 0] = base64_encode_byte(e0);
    result[j + 1] = base64_encode_byte(e1);
    result[j + 2] = base64_encode_byte(e2);
    result[j + 3] = base64_encode_byte(e3);
  }

  if (input_size % 3 == 2) {
    /* 1 padding character */
    auto e0{((input[i + 0] & std::byte{0xFC}) >> 2)};
    auto e1{(((input[i + 0] & std::byte{0x03}) << 4) |
             ((input[i + 1] & std::byte{0xF0}) >> 4))};
    auto e2{((input[i + 1] & std::byte{0x0F}) << 2)};
    result[j + 0] = base64_encode_byte(e0);
    result[j + 1] = base64_encode_byte(e1);
    result[j + 2] = base64_encode_byte(e2);
    result[j + 3] = '=';
  } else if (input_size % 3 == 1) {
    /* 2 padding characters */
    auto e0{((input[i + 0] & std::byte{0xFC}) >> 2)};
    auto e1{((input[i + 0] & std::byte{0x03}) << 4)};
    result[j + 0] = base64_encode_byte(e0);
    result[j + 1] = base64_encode_byte(e1);
    result[j + 2] = '=';
    result[j + 3] = '=';
  } else {
    /* No padding */
    auto e0{((input[i + 0] & std::byte{0xFC}) >> 2)};
    auto e1{(((input[i + 0] & std::byte{0x03}) << 4) |
             ((input[i + 1] & std::byte{0xF0}) >> 4))};
    auto e2{(((input[i + 1] & std::byte{0x0F}) << 2) |
             ((input[i + 2] & std::byte{0xC0}) >> 6))};
    auto e3{((input[i + 2] & std::byte{0x3F}) >> 0)};
    result[j + 0] = base64_encode_byte(e0);
    result[j + 1] = base64_encode_byte(e1);
    result[j + 2] = base64_encode_byte(e2);
    result[j + 3] = base64_encode_byte(e3);
  }

  return result;
}
};  // namespace sled
