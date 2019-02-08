/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "gtest/gtest.h"

#include "sled/arch.h"
#include "sled/x86arch.h"

class X86ArchTest : public ::testing::Test {
 protected:
  X86ArchTest() = default;
};

TEST_F(X86ArchTest, cpuid) {
  sled::x86::CPUID7 cpuid;

  auto ebx = cpuid.ebx();
  EXPECT_TRUE(ebx.is_set(sled::x86::CPUID7_EBX_FEATURE::BMI2));
  EXPECT_TRUE(ebx.is_set(sled::x86::CPUID7_EBX_FEATURE::AVX2));
  EXPECT_TRUE(ebx.is_set(sled::x86::CPUID7_EBX_FEATURE::AVX512f));
}

/**
 * Make sure result isn't optimized away.
 */
a_noinline bool avx512_supported() { return sled::avx512_hwarch::available(); }

TEST_F(X86ArchTest, arch_supported) {
  EXPECT_TRUE(sled::hwarch::available());
  EXPECT_TRUE(sled::avx2_hwarch::available());
  bool a = avx512_supported();
  bool b = avx512_supported();
  EXPECT_EQ(a, b);
}
