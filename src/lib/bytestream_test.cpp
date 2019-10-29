/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/bytestream.h"

#include "gtest/gtest.h"

class BytestreamTest : public ::testing::Test {
 protected:
  BytestreamTest() = default;

  void SetUp() override { fill(buffer_); }

  template <size_t SIZE>
  void fill(std::array<std::byte, SIZE> &buf) {
    for (int i = 0; i < sizeof(buffer_); i++) {
      buf[i] = std::byte(i);
    }
  }

  std::array<std::byte, 64> buffer_;
};

static auto test_bytestream_read = [](auto &bs) {
  uint64_t r = 0;
  r = bs.template be_pread<uint64_t>(0);
  EXPECT_EQ(0x0001020304050607, r);
  r = bs.template le_pread<uint64_t>(0);
  EXPECT_EQ(0x0706050403020100, r);
  uint32_t r2 = bs.template le_pread<uint32_t>(0);
  EXPECT_EQ(0x03020100, r2);
};

TEST_F(BytestreamTest, read) {
  sled::bytestream bs(buffer_.begin(), buffer_.end());
  test_bytestream_read(bs);
}

TEST_F(BytestreamTest, fixed_read) {
  sled::fixed_bytestream<64> bs;
  fill(bs.buf());

  test_bytestream_read(bs);
  uint64_t r = 0;
  r = bs.be_pread<uint64_t>(0);
  EXPECT_EQ(0x0001020304050607, r);
  r = bs.le_pread<uint64_t>(0);
  EXPECT_EQ(0x0706050403020100, r);
  uint32_t r2 = bs.le_pread<uint32_t>(0);
  EXPECT_EQ(0x03020100, r2);

  auto r3 = bs.be_pread(8, 0);
  EXPECT_EQ(0x0001020304050607, r3);

  auto r4 = bs.be_pread(6, 0);
  EXPECT_EQ(0x000102030405, r4);
}

template <typename T>
void test_bytestream_byte_read(T &bs) {
  std::byte r = bs.read(0x15);
  EXPECT_EQ(std::byte{0x15}, r);

  bs.pread(&r, sizeof(r), 0x14);
  EXPECT_EQ(std::byte{0x14}, r);
}

TEST_F(BytestreamTest, byte_read) {
  sled::bytestream bs(buffer_.begin(), buffer_.end());
  test_bytestream_byte_read(bs);
}

TEST_F(BytestreamTest, fixed_byte_read) {
  sled::fixed_bytestream<64> bs;
  fill(bs.buf());

  test_bytestream_byte_read(bs);
}

TEST_F(BytestreamTest, fixed_bits_read) {
  sled::fixed_bytestream<64> fbs;
  fill(fbs.buf());
  auto &bs = fbs.bs();

  EXPECT_EQ(0x00, bs.be_bits_pread<1>(0).v);
  EXPECT_EQ(0x01, bs.be_bits_pread<1>(15).v);
  EXPECT_EQ(0x1, bs.be_bits_pread<4>(16 * 8).v);
  auto v = bs.be_bits_pread<9>(16 * 8);
  EXPECT_EQ(9, v.width());
  EXPECT_EQ(0x20, v.v);
}
