/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/fmt.h"
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
  sled::IntFmt n1{sled::nibble_t{4}};
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
  sled::IntFmt n1{type{100}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(8, n1.w);

  EXPECT_EQ("100", sled::format(n1));
}

TEST_F(NumericTest, hex_fmt) {
  using type = uint8_t;
  sled::HexFmt n1{type{100}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(2, n1.w);
}

TEST_F(NumericTest, hex_tests) {
  using type = uint32_t;
  sled::HexFmt n1{type{100}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(8, n1.w);

  EXPECT_EQ("0x00000064", sled::format(n1));
}

TEST_F(NumericTest, alt_hex_tests) {
  using type = uint32_t;
  sled::HexFmt n1{type{100}, {sled::HexFormat::AltPrefix}};
  EXPECT_EQ(100, n1.v);
  EXPECT_EQ(8, n1.w);

  EXPECT_EQ("$00000064", sled::format(n1));
}

TEST_F(NumericTest, MiB_tests) {
  auto mib_test = [](std::string const &lhs, uint64_t rhs) {
    auto v = sled::IntFmt(rhs, {sled::IntegerFormat::MiB});
    EXPECT_EQ(lhs, sled::format(v));
  };
  mib_test("1MiB", 1024 * 1024);
  mib_test("1.0MiB", 1024 * 1024 + 1);
}

TEST_F(NumericTest, KiB_tests) {
  auto kib_test = [](std::string const &lhs, uint64_t rhs) {
    auto v = sled::IntFmt(rhs, {sled::IntegerFormat::KiB});
    EXPECT_EQ(lhs, sled::format(v));
  };
  kib_test("1KiB", 1024);
  kib_test("1.0KiB", 1024 + 1);
}
