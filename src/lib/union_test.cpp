/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/fmt.h"
#include "sled/numeric.h"
#include "sled/strong_int.h"
#include "sled/union.h"

#include "gtest/gtest.h"

class UnionTest : public ::testing::Test {
 protected:
  UnionTest() = default;
};

struct r8 : sled::StrongInt<uint8_t, r8> {
  using sled::StrongInt<uint8_t, r8>::StrongInt;
};

struct r16_bits {
  r16_bits() = default;
  explicit constexpr r16_bits(r8 h, r8 l) : l(l), h(h) {}
  r8 l;
  r8 h;
};

struct r8_bits {
  uint8_t A:4;
  uint8_t B:2;
  uint8_t C:1;
  uint8_t D:1;
};

TEST_F(UnionTest, typed_r8) {
  using r16 = sled::typed_union<r16_bits, uint16_t>;
  r16 r1 = 9;
  EXPECT_EQ(9, r1.v);

  using flags = sled::typed_union<r8_bits, r8>;
  flags f1 = 0xD1;
  EXPECT_EQ(1, f1.b.A);
}

struct r16_bytes {
  r16_bytes() = default;
  explicit constexpr r16_bytes(uint8_t h, uint8_t l) : l(l), h(h) {}
  uint8_t l;
  uint8_t h;
};

struct TestUnion final : sled::union_struct<r16_bytes, uint16_t, TestUnion> {
  using sled::union_struct<r16_bytes, uint16_t, TestUnion>::union_struct;

  friend inline std::ostream &operator<<(std::ostream &os,
                                         TestUnion const &obj) {
    os << sled::HexFmt(obj.v);
    return os;
  }

  friend inline std::string fmt_read(TestUnion const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

TEST_F(UnionTest, union_struct) {
  TestUnion tu{0};
  EXPECT_EQ(0, tu.get().l);
  EXPECT_EQ(0, tu.b.l);
  EXPECT_EQ(0, tu.v);
};

TEST_F(UnionTest, union_struct_assignment) {
  TestUnion tu{0};
  tu = 15;
  EXPECT_EQ(15, tu.v);
}

TEST_F(UnionTest, union_struct_string) {
  TestUnion tu{0};
  EXPECT_EQ("0x0000", sled::format(tu));
}
