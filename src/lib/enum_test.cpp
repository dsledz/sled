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

struct TestEnum final : sled::enum_struct<uint32_t, TestEnum> {
 public:
  static constexpr std::array<name_type, 4> names{
      std::make_pair(0, "Value0"), std::make_pair(1, "Value1"),
      std::make_pair(2, "Value2"), std::make_pair(3, "Value3")};

  using sled::enum_struct<uint32_t, TestEnum>::enum_struct;

  struct V;
};

struct TestEnum::V {
  static constexpr TestEnum Value0{0};
  static constexpr TestEnum Value1{1};
  static constexpr TestEnum Value2{2};
  static constexpr TestEnum Value3{3};
};

TEST_F(EnumTest, named_enum) {
  std::stringstream ss;
  TestEnum te{TestEnum::V::Value0};

  ss << te;

  EXPECT_EQ("Value0", ss.str());
}

TEST_F(EnumTest, string_to_enum) {
  auto te = TestEnum::from_string("Value0");
  EXPECT_EQ(te, TestEnum::V::Value0);

  EXPECT_THROW(TestEnum::from_string("Invalid"), sled::ConversionError);
}

TEST_F(EnumTest, unknown_enum) {
  std::stringstream ss;
  TestEnum te{100};

  ss << te;

  EXPECT_EQ("Invalid<100>", ss.str());
}

TEST_F(EnumTest, switch_statement) {
  auto te = TestEnum::V::Value0;
  int v = -1;

  switch (te) {
    case TestEnum::V::Value0:
      v = 0;
      break;
    case TestEnum::V::Value1:
      v = 1;
      break;
    case TestEnum::V::Value2:
      v = 2;
      break;
    case TestEnum::V::Value3:
      v = 3;
      break;
  };

  EXPECT_EQ(0, v);
}

TEST_F(EnumTest, iterate) {
  int count = 0;
  std::stringstream ss;
  for (auto t : TestEnum{}) {
    ss << t;
    count++;
  }
  EXPECT_EQ(4, count);
  EXPECT_EQ("Value0Value1Value2Value3", ss.str());
}
