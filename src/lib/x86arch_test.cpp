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

TEST_F(X86ArchTest, cpuid1) {
  sled::x86::CPUID1 cpuid;

  auto ecx = cpuid.ecx();
  EXPECT_TRUE(ecx.is_set(sled::x86::CPUID1_ECX_FEATURE::V::AVX));

  auto edx = cpuid.edx();
  EXPECT_TRUE(edx.is_set(sled::x86::CPUID1_EDX_FEATURE::V::SSE2));
}

TEST_F(X86ArchTest, cpuid7) {
  sled::x86::CPUID7 cpuid;

  auto ebx = cpuid.ebx();
  EXPECT_TRUE(ebx.is_set(sled::x86::CPUID7_EBX_FEATURE::V::BMI2));
  EXPECT_TRUE(ebx.is_set(sled::x86::CPUID7_EBX_FEATURE::V::AVX2));
  if (ebx.is_set(sled::x86::CPUID7_EBX_FEATURE::V::AVX512f)) {
      EXPECT_TRUE(sled::avx512_hwarch::available());
  } else {
      EXPECT_FALSE(sled::avx512_hwarch::available());
  }
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
