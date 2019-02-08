/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/enum.h"
#include "sled/platform.h"

namespace sled::x86 {

struct CPUID7_EBX_FEATURE final
    : sled::enum_struct<uint32_t, CPUID7_EBX_FEATURE> {
 public:
  static CPUID7_EBX_FEATURE const AVX2;
  static CPUID7_EBX_FEATURE const BMI2;
  static CPUID7_EBX_FEATURE const AVX512f;
  static CPUID7_EBX_FEATURE const SHA;

  static constexpr std::array<name_type, 4> names{
      std::make_pair(1 << 5, "AVX2"),
      std::make_pair(1 << 8, "BMI2"),
      std::make_pair(1 << 16, "AVX512f"),
      std::make_pair(1 << 29, "SHA"),
  };

  using sled::enum_struct<uint32_t, CPUID7_EBX_FEATURE>::enum_struct;
};

constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX2{1 << 5};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::BMI2{1 << 8};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512f{1 << 16};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::SHA{1 << 29};

struct CPUID7_EBX_FEATURES
    : public sled::enum_bitfield<CPUID7_EBX_FEATURE, CPUID7_EBX_FEATURES> {
  using sled::enum_bitfield<CPUID7_EBX_FEATURE,
                            CPUID7_EBX_FEATURES>::enum_bitfield;
};

template <int arg, typename EAX_RETURN = uint32_t,
          typename EBX_RETURN = uint32_t, typename ECX_RETURN = uint32_t,
          typename EDX_RETURN = uint32_t>
class cpuid_call {
 public:
  explicit cpuid_call() {
    __asm__("cpuid;"
            : "=a"(eax_), "=b"(ebx_), "=c"(ecx_), "=d"(edx_)
            : "a"(arg), "c"(0)
            :);
  }

  EAX_RETURN eax() { return EAX_RETURN{eax_}; }
  EBX_RETURN ebx() { return EBX_RETURN{ebx_}; }
  ECX_RETURN ecx() { return ECX_RETURN{ecx_}; }
  EDX_RETURN edx() { return EDX_RETURN{edx_}; }

 private:
  uint32_t eax_{0};
  uint32_t ebx_{0};
  uint32_t ecx_{0};
  uint32_t edx_{0};
};

using CPUID7 = cpuid_call<7, uint32_t, CPUID7_EBX_FEATURES, uint32_t, uint32_t>;

}  // namespace sled::x86
