/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/enum.h"
#include "sled/platform.h"

namespace sled::x86 {

struct CPUID1_ECX_FEATURE final
    : sled::enum_struct<uint32_t, CPUID1_ECX_FEATURE> {
 public:
  static constexpr std::array<name_type, 30> names{
      std::make_pair(1 << 0, "SSE3"),
      std::make_pair(1 << 1, "PCLMULQDQ"),
      std::make_pair(1 << 2, "DTES64"),
      std::make_pair(1 << 3, "MONITOR"),
      std::make_pair(1 << 4, "DS_CPL"),
      std::make_pair(1 << 5, "VMX"),
      std::make_pair(1 << 6, "SMX"),
      std::make_pair(1 << 7, "EST"),
      std::make_pair(1 << 8, "TM2"),
      std::make_pair(1 << 9, "SSSE3"),
      std::make_pair(1 << 10, "CNXT_ID"),
      std::make_pair(1 << 11, "SDBG"),
      std::make_pair(1 << 12, "FMA"),
      std::make_pair(1 << 13, "CX16"),
      std::make_pair(1 << 14, "XTPR"),
      std::make_pair(1 << 15, "PCDM"),
      std::make_pair(1 << 17, "PCID"),
      std::make_pair(1 << 18, "DCA"),
      std::make_pair(1 << 19, "SSE41"),
      std::make_pair(1 << 20, "SSE42"),
      std::make_pair(1 << 21, "X2APIC"),
      std::make_pair(1 << 22, "MOVBE"),
      std::make_pair(1 << 23, "POPCNT"),
      std::make_pair(1 << 24, "TSC_DEADLINE"),
      std::make_pair(1 << 25, "AES"),
      std::make_pair(1 << 26, "XSAVE"),
      std::make_pair(1 << 27, "OSXSAVE"),
      std::make_pair(1 << 28, "AVX"),
      std::make_pair(1 << 29, "F16C"),
      std::make_pair(1 << 30, "RDRND"),
  };

  using sled::enum_struct<uint32_t, CPUID1_ECX_FEATURE>::enum_struct;

  struct V;
};

struct CPUID1_ECX_FEATURE::V {
  static constexpr CPUID1_ECX_FEATURE const SSE3{1 << 0};
  static constexpr CPUID1_ECX_FEATURE const PCLMULQDQ{1 << 1};
  static constexpr CPUID1_ECX_FEATURE const DTES64{1 << 2};
  static constexpr CPUID1_ECX_FEATURE const MONITOR{1 << 3};
  static constexpr CPUID1_ECX_FEATURE const DS_CPL{1 << 4};
  static constexpr CPUID1_ECX_FEATURE const VMX{1 << 5};
  static constexpr CPUID1_ECX_FEATURE const SMX{1 << 6};
  static constexpr CPUID1_ECX_FEATURE const EST{1 << 7};
  static constexpr CPUID1_ECX_FEATURE const TM2{1 << 8};
  static constexpr CPUID1_ECX_FEATURE const SSSE3{1 << 9};
  static constexpr CPUID1_ECX_FEATURE const CNXT_ID{1 << 10};
  static constexpr CPUID1_ECX_FEATURE const SDBG{1 << 11};
  static constexpr CPUID1_ECX_FEATURE const FMA{1 << 12};
  static constexpr CPUID1_ECX_FEATURE const CX16{1 << 13};
  static constexpr CPUID1_ECX_FEATURE const XTPR{1 << 14};
  static constexpr CPUID1_ECX_FEATURE const PCDM{1 << 15};
  static constexpr CPUID1_ECX_FEATURE const PCID{1 << 17};
  static constexpr CPUID1_ECX_FEATURE const DCA{1 << 18};
  static constexpr CPUID1_ECX_FEATURE const SSE41{1 << 19};
  static constexpr CPUID1_ECX_FEATURE const SSE42{1 << 20};
  static constexpr CPUID1_ECX_FEATURE const X2APIC{1 << 21};
  static constexpr CPUID1_ECX_FEATURE const MOVBE{1 << 22};
  static constexpr CPUID1_ECX_FEATURE const POPCNT{1 << 23};
  static constexpr CPUID1_ECX_FEATURE const TSC_DEADLINE{1 << 24};
  static constexpr CPUID1_ECX_FEATURE const AES{1 << 25};
  static constexpr CPUID1_ECX_FEATURE const XSAVE{1 << 26};
  static constexpr CPUID1_ECX_FEATURE const OSXSAVE{1 << 27};
  static constexpr CPUID1_ECX_FEATURE const AVX{1 << 28};
  static constexpr CPUID1_ECX_FEATURE const F16C{1 << 29};
  static constexpr CPUID1_ECX_FEATURE const RDRND{1 << 30};
};

struct CPUID1_ECX_FEATURES
    : public sled::flags_struct<CPUID1_ECX_FEATURE, CPUID1_ECX_FEATURES> {
  using sled::flags_struct<CPUID1_ECX_FEATURE,
                           CPUID1_ECX_FEATURES>::flags_struct;
};

struct CPUID1_EDX_FEATURE final
    : sled::enum_struct<uint32_t, CPUID1_EDX_FEATURE> {
 public:

  static constexpr std::array<name_type, 30> names{
      std::make_pair(1 << 0, "FPU"),    std::make_pair(1 << 1, "VME"),
      std::make_pair(1 << 2, "DE"),     std::make_pair(1 << 3, "PSE"),
      std::make_pair(1 << 4, "TSC"),    std::make_pair(1 << 5, "MSR"),
      std::make_pair(1 << 6, "PAE"),    std::make_pair(1 << 7, "MCE"),
      std::make_pair(1 << 8, "CX8"),    std::make_pair(1 << 9, "APIC"),
      std::make_pair(1 << 11, "SEP"),   std::make_pair(1 << 12, "MTRR"),
      std::make_pair(1 << 13, "PGE"),   std::make_pair(1 << 14, "MCA"),
      std::make_pair(1 << 15, "CMOV"),  std::make_pair(1 << 16, "PAT"),
      std::make_pair(1 << 17, "PSE36"), std::make_pair(1 << 18, "PSN"),
      std::make_pair(1 << 19, "CLFSH"), std::make_pair(1 << 21, "DS"),
      std::make_pair(1 << 22, "ACPI"),  std::make_pair(1 << 23, "MMX"),
      std::make_pair(1 << 24, "FXSR"),  std::make_pair(1 << 25, "SSE"),
      std::make_pair(1 << 26, "SSE2"),  std::make_pair(1 << 27, "SS"),
      std::make_pair(1 << 28, "HTT"),   std::make_pair(1 << 29, "TM"),
      std::make_pair(1 << 30, "IA64"),  std::make_pair(1 << 31, "PBE"),
  };
  using sled::enum_struct<uint32_t, CPUID1_EDX_FEATURE>::enum_struct;

  struct V;
};

struct CPUID1_EDX_FEATURE::V {
  static constexpr CPUID1_EDX_FEATURE FPU{1 << 0};
  static constexpr CPUID1_EDX_FEATURE VME{1 << 1};
  static constexpr CPUID1_EDX_FEATURE DE{1 << 2};
  static constexpr CPUID1_EDX_FEATURE PSE{1 << 3};
  static constexpr CPUID1_EDX_FEATURE TSC{1 << 4};
  static constexpr CPUID1_EDX_FEATURE MSR{1 << 5};
  static constexpr CPUID1_EDX_FEATURE PAE{1 << 6};
  static constexpr CPUID1_EDX_FEATURE MCE{1 << 7};
  static constexpr CPUID1_EDX_FEATURE CX8{1 << 8};
  static constexpr CPUID1_EDX_FEATURE APIC{1 << 9};
  static constexpr CPUID1_EDX_FEATURE SEP{1 << 11};
  static constexpr CPUID1_EDX_FEATURE MTRR{1 << 12};
  static constexpr CPUID1_EDX_FEATURE PGE{1 << 13};
  static constexpr CPUID1_EDX_FEATURE MCA{1 << 14};
  static constexpr CPUID1_EDX_FEATURE CMOV{1 << 15};
  static constexpr CPUID1_EDX_FEATURE PAT{1 << 16};
  static constexpr CPUID1_EDX_FEATURE PSE36{1 << 17};
  static constexpr CPUID1_EDX_FEATURE PSN{1 << 18};
  static constexpr CPUID1_EDX_FEATURE CLFSH{1 << 19};
  static constexpr CPUID1_EDX_FEATURE DS{1 << 21};
  static constexpr CPUID1_EDX_FEATURE ACPI{1 << 22};
  static constexpr CPUID1_EDX_FEATURE MMX{1 << 23};
  static constexpr CPUID1_EDX_FEATURE FXSR{1 << 24};
  static constexpr CPUID1_EDX_FEATURE SSE{1 << 25};
  static constexpr CPUID1_EDX_FEATURE SSE2{1 << 26};
  static constexpr CPUID1_EDX_FEATURE SS{1 << 27};
  static constexpr CPUID1_EDX_FEATURE HTT{1 << 28};
  static constexpr CPUID1_EDX_FEATURE TM{1 << 29};
  static constexpr CPUID1_EDX_FEATURE IA64{1 << 30};
  static constexpr CPUID1_EDX_FEATURE PBE{1 << 31};
};

struct CPUID1_EDX_FEATURES
    : public sled::flags_struct<CPUID1_EDX_FEATURE, CPUID1_EDX_FEATURES> {
  using sled::flags_struct<CPUID1_EDX_FEATURE,
                           CPUID1_EDX_FEATURES>::flags_struct;
};

struct CPUID7_EBX_FEATURE final
    : sled::enum_struct<uint32_t, CPUID7_EBX_FEATURE> {
 public:
  static constexpr std::array<name_type, 14> names{
      std::make_pair(1 << 2, "SGX"),       std::make_pair(1 << 4, "HLE"),
      std::make_pair(1 << 5, "AVX2"),      std::make_pair(1 << 8, "BMI2"),
      std::make_pair(1 << 11, "RTM"),      std::make_pair(1 << 16, "AVX512f"),
      std::make_pair(1 << 17, "AVX512dq"), std::make_pair(1 << 18, "RDSEED"),
      std::make_pair(1 << 26, "AVX512pf"), std::make_pair(1 << 27, "AVX512er"),
      std::make_pair(1 << 28, "AVX512cd"), std::make_pair(1 << 29, "SHA"),
      std::make_pair(1 << 30, "AVX512bw"), std::make_pair(1 << 31, "AVX512vl"),
  };

  using sled::enum_struct<uint32_t, CPUID7_EBX_FEATURE>::enum_struct;

  struct V;
};

struct CPUID7_EBX_FEATURE::V {
  static constexpr CPUID7_EBX_FEATURE SGX{1 << 2};
  static constexpr CPUID7_EBX_FEATURE HLE{1 << 4};
  static constexpr CPUID7_EBX_FEATURE AVX2{1 << 5};
  static constexpr CPUID7_EBX_FEATURE BMI2{1 << 8};
  static constexpr CPUID7_EBX_FEATURE RTM{1 << 11};
  static constexpr CPUID7_EBX_FEATURE AVX512f{1 << 16};
  static constexpr CPUID7_EBX_FEATURE AVX512dq{1 << 17};
  static constexpr CPUID7_EBX_FEATURE RDSEED{1 << 18};
  static constexpr CPUID7_EBX_FEATURE AVX512pf{1 << 26};
  static constexpr CPUID7_EBX_FEATURE AVX512er{1 << 27};
  static constexpr CPUID7_EBX_FEATURE AVX512cd{1 << 28};
  static constexpr CPUID7_EBX_FEATURE SHA{1 << 29};
  static constexpr CPUID7_EBX_FEATURE AVX512bw{1 << 30};
  static constexpr CPUID7_EBX_FEATURE AVX512vl{1 << 31};
};

struct CPUID7_EBX_FEATURES
    : public sled::flags_struct<CPUID7_EBX_FEATURE, CPUID7_EBX_FEATURES> {
  using sled::flags_struct<CPUID7_EBX_FEATURE,
                           CPUID7_EBX_FEATURES>::flags_struct;
};

template <int arg, typename EAX_RETURN = uint32_t,
          typename EBX_RETURN = uint32_t, typename ECX_RETURN = uint32_t,
          typename EDX_RETURN = uint32_t>
class cpuid_call {
 public:
  explicit cpuid_call() {
#ifdef WIN32
    __cpuidex(reinterpret_cast<int *>(info_.data()), arg, 0);
#else
    __asm__("cpuid;"
            : "=a"(info_[0]), "=b"(info_[1]), "=c"(info_[2]), "=d"(info_[3])
            : "a"(arg), "c"(0)
            :);
#endif
  }

  EAX_RETURN eax() { return EAX_RETURN{info_[0]}; }
  EBX_RETURN ebx() { return EBX_RETURN{info_[1]}; }
  ECX_RETURN ecx() { return ECX_RETURN{info_[2]}; }
  EDX_RETURN edx() { return EDX_RETURN{info_[3]}; }

 private:
  std::array<uint32_t, 4> info_{};
};

using CPUID1 =
    cpuid_call<1, uint32_t, uint32_t, CPUID1_ECX_FEATURES, CPUID1_EDX_FEATURES>;
using CPUID7 = cpuid_call<7, uint32_t, CPUID7_EBX_FEATURES, uint32_t, uint32_t>;

}  // namespace sled::x86
