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

class atomic_flagsTest : public ::testing::Test {
 public:
  enum class TestEnum {
    Bit0 = 0x00,
    Bit1 = 0x01,
    Bit2 = 0x02,
    Bit3 = 0x04,
    Bit4 = 0x08
  };
  using atomic_flags_t = sled::atomic_flags<TestEnum>;
  using flags_t = sled::flags<TestEnum>;

 protected:
  atomic_flagsTest() = default;

  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(atomic_flagsTest, bitwise_or_equal) {
  auto f1 = atomic_flags_t{TestEnum::Bit1};
  f1 |= atomic_flags_t{TestEnum::Bit2};

  EXPECT_EQ(0x03, f1.get());
}

TEST_F(atomic_flagsTest, bitwise_and_equal) {
  auto f1 = atomic_flags_t{TestEnum::Bit1};
  f1 &= atomic_flags_t{TestEnum::Bit1, TestEnum::Bit2};

  EXPECT_EQ(0x01, f1.get());
}

TEST_F(atomic_flagsTest, bitwise_or) {
  auto f1 = atomic_flags_t{TestEnum::Bit1};
  auto f2 = atomic_flags_t{TestEnum::Bit2};
  auto f3 = f1 | f2;

  EXPECT_EQ(0x03, f3.get());
}

TEST_F(atomic_flagsTest, update) {
  auto f1 = atomic_flags_t{TestEnum::Bit1};
  auto result = f1.update({TestEnum::Bit2}, {TestEnum::Bit1});
  EXPECT_EQ((flags_t{TestEnum::Bit2}), result);
}

TEST_F(atomic_flagsTest, update_cond) {
  auto f1 = atomic_flags_t{TestEnum::Bit1};
  auto result = f1.update_cond({TestEnum::Bit2}, {TestEnum::Bit1});
  EXPECT_TRUE(result.first);
  EXPECT_EQ(flags_t{TestEnum::Bit2}, result.second);

  result = f1.update_cond({TestEnum::Bit2}, {TestEnum::Bit1});
  EXPECT_FALSE(result.first);
  EXPECT_EQ((flags_t{TestEnum::Bit2}), result.second);

  auto f2 = atomic_flags_t{TestEnum::Bit1, TestEnum::Bit3};
  result = f2.update_cond({TestEnum::Bit2}, {TestEnum::Bit1});
  EXPECT_TRUE(result.first);
  EXPECT_EQ((flags_t{TestEnum::Bit2, TestEnum::Bit3}), result.second);
}

TEST_F(atomic_flagsTest, set_cond) {
  auto f1 = atomic_flags_t{TestEnum::Bit1};
  auto result = f1.set_cond({TestEnum::Bit2}, {TestEnum::Bit2});
  EXPECT_TRUE(result.first);
  EXPECT_EQ((flags_t{TestEnum::Bit1, TestEnum::Bit2}), result.second);

  result = f1.set_cond({TestEnum::Bit2}, {TestEnum::Bit2});
  EXPECT_FALSE(result.first);
  EXPECT_EQ((flags_t{TestEnum::Bit1, TestEnum::Bit2}), result.second);

  result = f1.set_cond({TestEnum::Bit3}, {TestEnum::Bit4});
  EXPECT_TRUE(result.first);
  EXPECT_EQ((flags_t{TestEnum::Bit1, TestEnum::Bit2, TestEnum::Bit3}),
            result.second);
}
