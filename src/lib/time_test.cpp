/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/time.h"

#include "gtest/gtest.h"

class TimeTest : public ::testing::Test {
 protected:
  TimeTest() = default;
};

TEST_F(TimeTest, constructor) {
  EXPECT_EQ(100, sled::nsec{100}.v);
  EXPECT_EQ(100, sled::usec{100}.v);
  EXPECT_EQ(100, sled::msec{100}.v);
  EXPECT_EQ(100, sled::sec{100}.v);
}

TEST_F(TimeTest, arithmetic) {
  auto t = sled::time{sled::nsec{100}};
  EXPECT_EQ(100ll, t.v);
  auto t0 = sled::time{sled::usec{100}};
  EXPECT_EQ(100'000ll, t0.v);
  auto t1 = sled::time{sled::msec{100}};
  EXPECT_EQ(100'000'000ll, t1.v);
  t += t0;
  EXPECT_EQ(100'100ll, t.v);
  t += t1;
  EXPECT_EQ(100'100'100ll, t.v);
  EXPECT_EQ(100'100'000ll, (t0 + t1).v);
  EXPECT_EQ(100ll, (t0 / 1000).v);
}

TEST_F(TimeTest, from) {
  EXPECT_EQ(100ul, sled::time::from_nsec(100).v);
  EXPECT_EQ(100'000ul, sled::time::from_usec(100).v);
  EXPECT_EQ(100'000'000ul, sled::time::from_msec(100).v);
  EXPECT_EQ(100'000'000'000ul, sled::time::from_sec(100).v);
}

TEST_F(TimeTest, formatter) {
  {
    std::stringstream ss;
    ss << sled::time(sled::nsec{100});
    EXPECT_EQ("100ns", ss.str());
  }
  {
    std::stringstream ss;
    ss << sled::time(sled::usec{100});
    EXPECT_EQ("100us", ss.str());
  }
  {
    std::stringstream ss;
    ss << sled::time(sled::msec{100});
    EXPECT_EQ("100ms", ss.str());
  }
  {
    std::stringstream ss;
    auto t = sled::time(sled::msec{100});
    t += sled::time(sled::nsec{100});
    ss << t;
    EXPECT_EQ("100.0ms", ss.str());
  }
  {
    std::stringstream ss;
    ss << sled::time(sled::sec{100});
    EXPECT_EQ("100s", ss.str());
  }
}

TEST_F(TimeTest, sw) {
  sled::stopwatch sw;

  auto t0 = sled::time_zero;
  auto t1 = sw.split();
  EXPECT_GT(t1, t0);
}
