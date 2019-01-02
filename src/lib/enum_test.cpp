/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/enum.h"

#include "gtest/gtest.h"

class EnumTest : public ::testing::Test {
 protected:
  EnumTest() = default;
};

struct TestEnum final : sled::StrongEnum<uint32_t, TestEnum> {
 public:
  static TestEnum const Value0;
  static TestEnum const Value1;
  static TestEnum const Value2;
  static TestEnum const Value3;

  static constexpr std::array<const char *, 4> names{"Value0", "Value1",
                                                     "Value2", "Value3"};

 public:
  using sled::StrongEnum<uint32_t, TestEnum>::StrongEnum;
};

constexpr TestEnum const TestEnum::Value0{0};
constexpr TestEnum const TestEnum::Value1{1};
constexpr TestEnum const TestEnum::Value2{2};
constexpr TestEnum const TestEnum::Value3{3};

TEST_F(EnumTest, named_enum) {
  std::stringstream ss;
  TestEnum te{TestEnum::Value0};

  ss << te;

  EXPECT_EQ("Value0", ss.str());
}

TEST_F(EnumTest, unknown_enum) {
  std::stringstream ss;
  TestEnum te{100};

  ss << te;

  EXPECT_EQ("Invalid<100>", ss.str());
}
