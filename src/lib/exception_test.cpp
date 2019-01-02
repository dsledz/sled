/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/exception.h"

#include "gtest/gtest.h"

class ExceptionTest : public ::testing::Test {};

class TestException final : public sled::Exception {
 public:
  explicit TestException(const std::string &msg) : Exception(msg) {}
};

TEST_F(ExceptionTest, basic) {
  TestException ex("example");
  EXPECT_EQ(std::string{"example"}, std::string{ex.what()});
}

TEST_F(ExceptionTest, conversion_error) {
  sled::ConversionError ce("time");
  EXPECT_EQ(std::string{"ConversionError: time"}, std::string{ce.what()});
  sled::ConversionError ce1("time", "now");
  EXPECT_EQ(std::string{"ConversionError: time, \"now\""},
            std::string{ce1.what()});
}
