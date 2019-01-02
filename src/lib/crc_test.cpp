/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/crc.h"

#include "gtest/gtest.h"

class Crc32cTest : public ::testing::Test {
 protected:
  Crc32cTest() = default;

  void SetUp() override {
    tv_u8 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    tv_u16 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    tv_u32 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    tv_u64 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  }

  void TearDown() override {}

  std::vector<uint8_t> tv_u8;
  std::vector<uint16_t> tv_u16;
  std::vector<uint32_t> tv_u32;
  std::vector<uint64_t> tv_u64;
};

TEST_F(Crc32cTest, calculate_vector) {
  sled::crc32c zero{0};
  sled::crc32c nonzero{17};
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u8));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u8, nonzero));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u16));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u16, nonzero));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u32));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u32, nonzero));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u64));
  EXPECT_FALSE(zero == sled::calculate_crc32c(tv_u64, nonzero));
}
