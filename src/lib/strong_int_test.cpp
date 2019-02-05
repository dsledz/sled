/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/strong_int.h"

#include "gtest/gtest.h"

class StrongIntTest : public ::testing::Test {
 protected:
  StrongIntTest() = default;

  void SetUp() override {}
  void TearDown() override {}
};

struct TestInt : public sled::StrongInt<uint32_t, TestInt> {
  using sled::StrongInt<uint32_t, TestInt>::StrongInt;

  friend std::ostream &operator<<(std::ostream &os, TestInt const &rhs) {
    os << static_cast<uint32_t>(rhs.v);
    return os;
  }
};

TEST_F(StrongIntTest, trivial) {
  static_assert(std::is_trivially_constructible_v<TestInt>);
  static_assert(std::is_default_constructible_v<TestInt>);
}

TEST_F(StrongIntTest, inherited) {
  TestInt ti1{100};
  TestInt ti2{10};

  EXPECT_EQ(100, ti1.get());

  static_assert(std::is_same_v<decltype(ti1 + ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 - ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 * ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 / ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 % ti2), decltype(ti1)>,
                "Wrong deduced type");

  static_assert(std::is_same_v<decltype(ti1 += ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 -= ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 *= ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 /= ti2), decltype(ti1)>,
                "Wrong deduced type");
  static_assert(std::is_same_v<decltype(ti1 %= ti2), decltype(ti1)>,
                "Wrong deduced type");

  ti1 += ti2;
  EXPECT_EQ(110, ti1.get());

  std::stringstream ss;
  ss << ti1;
  EXPECT_EQ("110", ss.str());
}
