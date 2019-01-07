/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/atomic.h"
#include "sled/channel.h"
#include "sled/coroutine.h"
#include "sled/threadpool.h"

#include "gtest/gtest.h"

class AtomicBitFieldTest : public ::testing::Test {
 public:
  enum class TestEnum { Bit0 = 0x00, Bit1 = 0x01, Bit2 = 0x02, Bit3 = 0x04 };
  using atomic_bit_field = sled::AtomicBitField<TestEnum>;
  using bit_field = sled::BitField<TestEnum>;

 protected:
  AtomicBitFieldTest() = default;

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(AtomicBitFieldTest, bitwise_or_equal) {
  auto f1 = atomic_bit_field{TestEnum::Bit1};
  f1 |= atomic_bit_field{TestEnum::Bit2};

  EXPECT_EQ(0x03, f1.get());
}

TEST_F(AtomicBitFieldTest, bitwise_and_equal) {
  auto f1 = atomic_bit_field{TestEnum::Bit1};
  f1 &= atomic_bit_field{TestEnum::Bit1, TestEnum::Bit2};

  EXPECT_EQ(0x01, f1.get());
}

TEST_F(AtomicBitFieldTest, bitwise_or) {
  auto f1 = atomic_bit_field{TestEnum::Bit1};
  auto f2 = atomic_bit_field{TestEnum::Bit2};
  auto f3 = f1 | f2;

  EXPECT_EQ(0x03, f3.get());
}

TEST_F(AtomicBitFieldTest, update) {
  auto f1 = atomic_bit_field{TestEnum::Bit1};
  f1.update({TestEnum::Bit2}, {TestEnum::Bit1});
}