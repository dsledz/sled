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

enum class FlagNames { Flag1 = 0x01, Flag2 = 0x02, Flag3 = 0x04 };

TEST_F(EnumTest, simple_flags) {
  sled::flags<FlagNames> flag{FlagNames::Flag1};

  EXPECT_EQ(0x01, flag.get());

  flag |= FlagNames::Flag2;
  EXPECT_EQ(0x03, flag.get());
}

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

// The following should fail because operator<< for Tag is deleted.
#ifdef COMPILE_ERROR
TEST_F(EnumTest, named_enum) {
  std::stringstream ss;
  TestEnum te{TestEnum::V::Value0};

  ss << te;

  EXPECT_EQ("Value0", ss.str());
}
#endif

TEST_F(EnumTest, named_enum) {
  std::stringstream ss;
  TestEnum te{TestEnum::V::Value0};

  ss << te.fmt();

  EXPECT_EQ("Value0", ss.str());
}

TEST_F(EnumTest, auto_fmt) {
  TestEnum te{TestEnum::V::Value0};
  auto f = sled::format(te);

  EXPECT_EQ("Value0", f);
}

TEST_F(EnumTest, string_to_enum) {
  auto te = TestEnum::from_string("Value0");
  EXPECT_EQ(te, TestEnum::V::Value0);

  EXPECT_THROW(TestEnum::from_string("Invalid"), sled::ConversionError);
}

TEST_F(EnumTest, choices) {
  std::stringstream ss;
  TestEnum::choices(ss);

  EXPECT_EQ("[Value0, Value1, Value2, Value3]", ss.str());
}

TEST_F(EnumTest, unknown_enum) {
  std::stringstream ss;
  TestEnum te{100};

  ss << te.fmt();

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
    ss << t.fmt();
    count++;
  }
  EXPECT_EQ(4, count);
  EXPECT_EQ("Value0Value1Value2Value3", ss.str());
}

struct TestFlag final : sled::enum_struct<uint32_t, TestFlag> {
  static constexpr std::array<name_type, 4> names {
    std::make_pair(0x01, "Flag1"),
    std::make_pair(0x02, "Flag2"),
    std::make_pair(0x04, "Flag3"),
    std::make_pair(0x08, "Flag4"),
  };

  using enum_struct::enum_struct;

  struct V;
};

struct TestFlag::V {
  static constexpr TestFlag Flag1{0x01};
  static constexpr TestFlag Flag2{0x02};
  static constexpr TestFlag Flag3{0x04};
  static constexpr TestFlag Flag4{0x08};
};

struct TestFlags final : sled::flags_struct<TestFlag, TestFlags> {
  using flags_struct::flags_struct;
};

TEST_F(EnumTest, flags) {
  TestFlags flags{TestFlag::V::Flag1, TestFlag::V::Flag2};

  EXPECT_EQ(0x01 | 0x02, flags.get());
}

TEST_F(EnumTest, flags_choices) {
  std::stringstream ss;

  TestFlags::choices(ss);

  EXPECT_EQ("[Flag1|Flag2|Flag3|Flag4]", ss.str());
}

TEST_F(EnumTest, flags_from_string) {
  EXPECT_EQ(0x01, TestFlags::from_string("Flag1").get());
  EXPECT_EQ(0x03, TestFlags::from_string("Flag1|Flag2").get());
  EXPECT_EQ(0x05, TestFlags::from_string("Flag3|Flag1").get());

  EXPECT_THROW(TestFlags::from_string("Flg1"), sled::ConversionError);

  EXPECT_EQ(0x00, TestFlags::from_string("").get());
}
