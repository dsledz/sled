/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "gtest/gtest.h"

#include "sled/base64.h"

using namespace sled;
using bvec = std::vector<std::byte>;

constexpr static inline std::byte operator"" _b(unsigned long long n) {
  return std::byte(n);
}

TEST(Base64Test, encode) {
  EXPECT_EQ("AA==", base64_encode(bvec{0x00_b}));
  EXPECT_EQ("AAA=", base64_encode(bvec{0x00_b, 0x00_b}));
  EXPECT_EQ("AAAA", base64_encode(bvec{0x00_b, 0x00_b, 0x00_b}));
  EXPECT_EQ("AAAAAA==", base64_encode(bvec{0x00_b, 0x00_b, 0x00_b, 0x00_b}));
  EXPECT_EQ("////", base64_encode(bvec{0xFF_b, 0xFF_b, 0xFF_b}));
}

TEST(Base64Test, decode) {
  EXPECT_EQ((bvec{0x00_b}), base64_decode("AA=="));
  EXPECT_EQ((bvec{0x00_b, 0x00_b}), base64_decode("AAA="));
  EXPECT_EQ((bvec{0x00_b, 0x00_b, 0x00_b}), base64_decode("AAAA"));
  EXPECT_EQ((bvec{0xFF_b, 0xFF_b, 0xFF_b}), base64_decode("////"));
}

TEST(Base64Test, first_char) {
  std::string base =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  ASSERT_EQ(64, base.size());

  for (uint8_t i = 0; i < 64; i++) {
    std::vector<std::byte> expected(1);
    expected[0] = std::byte(i << 2);
    std::string str = "AA==";
    str[0] = base[i];
    auto actual = base64_decode(str);
    EXPECT_EQ(expected, actual);
  }
}
