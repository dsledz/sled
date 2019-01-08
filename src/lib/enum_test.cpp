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

  static constexpr std::array<name_type, 4> names{
      std::make_pair(0, "Value0"), std::make_pair(1, "Value1"),
      std::make_pair(2, "Value2"), std::make_pair(3, "Value3")};

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

TEST_F(EnumTest, string_to_enum) {
  auto te = TestEnum::from_string("Value0");
  EXPECT_EQ(te, TestEnum::Value0);

  EXPECT_THROW(TestEnum::from_string("Invalid"), sled::ConversionError);
}

TEST_F(EnumTest, unknown_enum) {
  std::stringstream ss;
  TestEnum te{100};

  ss << te;

  EXPECT_EQ("Invalid<100>", ss.str());
}
