/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/bitfield.h"

#include "gtest/gtest.h"

class BitfieldTest : public ::testing::Test {
 protected:
  BitfieldTest() = default;
};

struct hidden_struct {
  uint16_t f1 : 3;
  uint16_t f2 : 4;
  uint16_t f3 : 2;
  uint16_t f4 : 7;
};

struct TestBitfield final
    : sled::struct_bitfield<uint16_t, hidden_struct, TestBitfield> {
 public:
  static constexpr std::array<field_description, 4> fields{
      make_field(0, 2, "f1"),
      make_field(3, 6, "f2"),
      make_field(7, 8, "f3"),
      make_field(9, 15, "f4"),
  };

  using sled::struct_bitfield<uint16_t, hidden_struct,
                              TestBitfield>::struct_bitfield;
};

TEST_F(BitfieldTest, construct) {
  TestBitfield f;
  f.b.f1 = 2;
  f.b.f2 = 2;
  f.b.f3 = 2;
  f.b.f4 = 2;

  EXPECT_EQ(2, f.b.f1);

  EXPECT_EQ("f1:2, f2:2, f3:2, f4:2", to_string(f));
}

struct TestFlags {
  uint8_t C : 1;  /**< Carry Flag */
  uint8_t Z : 1;  /**< Zero Flag */
  uint8_t I : 1;  /**< emu::Interrupt Inhibit */
  uint8_t D : 1;  /**< Decimal flag */
  uint8_t _B : 1; /**< Break flag (virtual) */
  uint8_t _E : 1; /**< emu::Interrupt flag (virtual) */
  uint8_t V : 1;  /**< Overflow */
  uint8_t N : 1;  /**< Negative */
};

struct TestFlagsBitfield final
    : sled::flags_bitfield<uint8_t, TestFlags, TestFlagsBitfield> {
 public:
  static constexpr std::array<field_description, 6> fields{
      make_field(0, "C"), make_field(1, "Z"), make_field(2, "I"),
      make_field(3, "D"), make_field(6, "V"), make_field(7, "N"),
  };

  using sled::flags_bitfield<uint8_t, TestFlags,
                             TestFlagsBitfield>::flags_bitfield;
};

TEST_F(BitfieldTest, with_flags) {
  TestFlagsBitfield f;
  f.b.Z = 1;
  f.b.D = 1;
  EXPECT_EQ("Z,D", to_string(f));
}

TEST_F(BitfieldTest, assignment) {
  TestFlagsBitfield f;
  f = 0x0A;
  EXPECT_EQ("Z,D", to_string(f));
}
