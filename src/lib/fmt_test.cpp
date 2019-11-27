/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/fmt.h"

#include "gtest/gtest.h"

class FmtTest : public ::testing::Test {
 protected:
  FmtTest() = default;

  void SetUp() override {}
  void TearDown() override {}
};

struct TestIntFmt {
 public:
  explicit TestIntFmt(int v) : v_(v) {}

  int v_{0};

  friend std::string fmt_read(const TestIntFmt &x) {
    std::stringstream ss;
    ss << x.v_;
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &os, const TestIntFmt &x) {
    os << x.v_;
    return os;
  }
};

TEST_F(FmtTest, format_wrappers) {
  sled::fmt f(TestIntFmt(1234));
  auto fn = [](const sled::fmt &fmt) -> std::string { return fmt_read(fmt); };

  EXPECT_EQ("1234", fn(f));
  EXPECT_EQ("1234", fn(TestIntFmt(1234)));

  EXPECT_EQ("hi", fn(std::string("hi")));
}

TEST_F(FmtTest, ostream_wrappers) {
  std::stringstream ss;
  ss << TestIntFmt(1234);
  EXPECT_EQ(ss.str(), "1234");
}

TEST_F(FmtTest, string_cast) {
  auto fn = [](const std::string &x) -> std::string { return x; };

  EXPECT_EQ("1234", fn(sled::fmt(TestIntFmt(1234))));
}

TEST_F(FmtTest, format_fn) {
  EXPECT_EQ("1234", sled::format(TestIntFmt(1234)));
  EXPECT_EQ("1234", sled::format(std::string("1234")));
  EXPECT_EQ("1234", sled::format("1234"));
}

TEST_F(FmtTest, format_string) {
  EXPECT_EQ("Hello, world C-137.",
            sled::format("Hello, world C", TestIntFmt(-137), "."));
}

TEST_F(FmtTest, format_os) {
  std::stringstream ss;
  sled::format_os(ss, "Hello, world C", TestIntFmt(-137), ".");
  EXPECT_EQ("Hello, world C-137.", ss.str());
}

#ifdef COMPILE_ERROR
TEST_F(FmtTest, format_compile_error) {
  // No conversion between int and sled::fmt
  EXPECT_EQ("1234", sled::format(1234));
}
#endif
