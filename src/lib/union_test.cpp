/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/numeric.h"
#include "sled/union.h"

#include "gtest/gtest.h"

class UnionTest : public ::testing::Test {
 protected:
  UnionTest() = default;
};

struct Bits16 {
  Bits16() = default;
  explicit constexpr Bits16(uint8_t h, uint8_t l) : l(l), h(h) {}
  uint8_t l;
  uint8_t h;
};

struct TestUnion final : sled::strong_union<Bits16, uint16_t, TestUnion> {
  friend inline std::ostream &operator<<(std::ostream &os,
                                         TestUnion const &obj) {
    os << sled::Hex(obj.v);
    return os;
  }

  friend inline std::string to_string(TestUnion const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

TEST_F(UnionTest, named_union) {
  TestUnion tu{0};
  EXPECT_EQ(0, tu.get().l);
  EXPECT_EQ(0, tu.v);
};

TEST_F(UnionTest, named_union_string) {
  TestUnion tu{0};
  EXPECT_EQ("0x00", to_string(tu));
}
