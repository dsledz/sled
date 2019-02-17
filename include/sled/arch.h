/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"

#if SLED_X86_64
#include "sled/x86arch.h"
#endif

namespace sled {

/**
 * Hardware architecture type.
 *
 * Used to select hardware specific algorithms.
 */
struct hwarch {
  static bool available() { return true; }
};

#if SLED_X86_64
/**
 * AVX2 hardware (Broadwell-ish)
 */
struct avx2_hwarch : public hwarch {
  static bool available() {
    sled::x86::CPUID7 cpuid;
    return cpuid.ebx().is_set(sled::x86::CPUID7_EBX_FEATURE::V::AVX2);
  }
};

/**
 * AVX512 hardware (Cannon-lake)
 */
struct avx512_hwarch : public avx2_hwarch {
  static bool available() {
    sled::x86::CPUID7 cpuid;
    return cpuid.ebx().is_set(sled::x86::CPUID7_EBX_FEATURE::V::AVX512f);
  }
};
#else
struct avx2_hwarch : public hwarch {
  constexpr bool available() { return false; }
};

struct avx512_hwarch : public hwarch {
  constexpr bool available() { return false; }
};
#endif

}  // namespace sled
