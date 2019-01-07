/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/numeric.h"
#include "sled/strong_int.h"

#include "gtest/gtest.h"

class NumericTest : public ::testing::Test {
 protected:
  NumericTest() = default;

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(NumericTest, nibble_size) {
  sled::Integer n1{sled::nibble_t{4}};
  EXPECT_EQ(4, n1.v);
  EXPECT_EQ(1, n1.w);
}

struct tag1 : public sled::StrongInt<uint32_t, tag1> {
  using sled::StrongInt<uint32_t, tag1>::StrongInt;
};

template <>
struct sled::__is_wrapped_integer_helper<tag1> : public std::true_type {};

TEST_F(NumericTest, strong_int_tests) {
  using type = tag1;
  sled::Integer n1{type{100}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(8, n1.w);

  EXPECT_EQ("100", to_string(n1));
}

TEST_F(NumericTest, hex_tests) {
  using type = uint32_t;
  sled::Hex n1{type{100}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(8, n1.w);

  EXPECT_EQ("0x00000064", to_string(n1));
}

TEST_F(NumericTest, alt_hex_tests) {
  using type = uint32_t;
  sled::AltHex n1{type{100}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(8, n1.w);

  EXPECT_EQ("$00000064", to_string(n1));
}
