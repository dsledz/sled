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
  static CPUID1_ECX_FEATURE const SSE3;
  static CPUID1_ECX_FEATURE const PCLMULQDQ;
  static CPUID1_ECX_FEATURE const DTES64;
  static CPUID1_ECX_FEATURE const MONITOR;
  static CPUID1_ECX_FEATURE const DS_CPL;
  static CPUID1_ECX_FEATURE const VMX;
  static CPUID1_ECX_FEATURE const SMX;
  static CPUID1_ECX_FEATURE const EST;
  static CPUID1_ECX_FEATURE const TM2;
  static CPUID1_ECX_FEATURE const SSSE3;
  static CPUID1_ECX_FEATURE const CNXT_ID;
  static CPUID1_ECX_FEATURE const SDBG;
  static CPUID1_ECX_FEATURE const FMA;
  static CPUID1_ECX_FEATURE const CX16;
  static CPUID1_ECX_FEATURE const XTPR;
  static CPUID1_ECX_FEATURE const PCDM;
  static CPUID1_ECX_FEATURE const PCID;
  static CPUID1_ECX_FEATURE const DCA;
  static CPUID1_ECX_FEATURE const SSE41;
  static CPUID1_ECX_FEATURE const SSE42;
  static CPUID1_ECX_FEATURE const X2APIC;
  static CPUID1_ECX_FEATURE const MOVBE;
  static CPUID1_ECX_FEATURE const POPCNT;
  static CPUID1_ECX_FEATURE const TSC_DEADLINE;
  static CPUID1_ECX_FEATURE const AES;
  static CPUID1_ECX_FEATURE const XSAVE;
  static CPUID1_ECX_FEATURE const OSXSAVE;
  static CPUID1_ECX_FEATURE const AVX;
  static CPUID1_ECX_FEATURE const F16C;
  static CPUID1_ECX_FEATURE const RDRND;

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
};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::SSE3{1 << 0};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::PCLMULQDQ{1 << 1};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::DTES64{1 << 2};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::MONITOR{1 << 3};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::DS_CPL{1 << 4};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::VMX{1 << 5};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::SMX{1 << 6};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::EST{1 << 7};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::TM2{1 << 8};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::SSSE3{1 << 9};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::CNXT_ID{1 << 10};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::SDBG{1 << 11};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::FMA{1 << 12};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::CX16{1 << 13};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::XTPR{1 << 14};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::PCDM{1 << 15};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::PCID{1 << 17};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::DCA{1 << 18};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::SSE41{1 << 19};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::SSE42{1 << 20};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::X2APIC{1 << 21};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::MOVBE{1 << 22};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::POPCNT{1 << 23};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::TSC_DEADLINE{1 << 24};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::AES{1 << 25};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::XSAVE{1 << 26};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::OSXSAVE{1 << 27};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::AVX{1 << 28};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::F16C{1 << 29};
constexpr CPUID1_ECX_FEATURE const CPUID1_ECX_FEATURE::RDRND{1 << 30};

struct CPUID1_ECX_FEATURES
    : public sled::flags_struct<CPUID1_ECX_FEATURE, CPUID1_ECX_FEATURES> {
  using sled::flags_struct<CPUID1_ECX_FEATURE,
                           CPUID1_ECX_FEATURES>::flags_struct;
};

struct CPUID1_EDX_FEATURE final
    : sled::enum_struct<uint32_t, CPUID1_EDX_FEATURE> {
 public:
  static CPUID1_EDX_FEATURE const FPU;
  static CPUID1_EDX_FEATURE const VME;
  static CPUID1_EDX_FEATURE const DE;
  static CPUID1_EDX_FEATURE const PSE;
  static CPUID1_EDX_FEATURE const TSC;
  static CPUID1_EDX_FEATURE const MSR;
  static CPUID1_EDX_FEATURE const PAE;
  static CPUID1_EDX_FEATURE const MCE;
  static CPUID1_EDX_FEATURE const CX8;
  static CPUID1_EDX_FEATURE const APIC;
  static CPUID1_EDX_FEATURE const SEP;
  static CPUID1_EDX_FEATURE const MTRR;
  static CPUID1_EDX_FEATURE const PGE;
  static CPUID1_EDX_FEATURE const MCA;
  static CPUID1_EDX_FEATURE const CMOV;
  static CPUID1_EDX_FEATURE const PAT;
  static CPUID1_EDX_FEATURE const PSE36;
  static CPUID1_EDX_FEATURE const PSN;
  static CPUID1_EDX_FEATURE const CLFSH;
  static CPUID1_EDX_FEATURE const DS;
  static CPUID1_EDX_FEATURE const ACPI;
  static CPUID1_EDX_FEATURE const MMX;
  static CPUID1_EDX_FEATURE const FXSR;
  static CPUID1_EDX_FEATURE const SSE;
  static CPUID1_EDX_FEATURE const SSE2;
  static CPUID1_EDX_FEATURE const SS;
  static CPUID1_EDX_FEATURE const HTT;
  static CPUID1_EDX_FEATURE const TM;
  static CPUID1_EDX_FEATURE const IA64;
  static CPUID1_EDX_FEATURE const PBE;

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
};

constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::FPU{1 << 0};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::VME{1 << 1};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::DE{1 << 2};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PSE{1 << 3};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::TSC{1 << 4};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::MSR{1 << 5};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PAE{1 << 6};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::MCE{1 << 7};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::CX8{1 << 8};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::APIC{1 << 9};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::SEP{1 << 11};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::MTRR{1 << 12};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PGE{1 << 13};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::MCA{1 << 14};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::CMOV{1 << 15};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PAT{1 << 16};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PSE36{1 << 17};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PSN{1 << 18};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::CLFSH{1 << 19};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::DS{1 << 21};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::ACPI{1 << 22};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::MMX{1 << 23};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::FXSR{1 << 24};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::SSE{1 << 25};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::SSE2{1 << 26};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::SS{1 << 27};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::HTT{1 << 28};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::TM{1 << 29};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::IA64{1 << 30};
constexpr CPUID1_EDX_FEATURE const CPUID1_EDX_FEATURE::PBE{1 << 31};

struct CPUID1_EDX_FEATURES
    : public sled::flags_struct<CPUID1_EDX_FEATURE, CPUID1_EDX_FEATURES> {
  using sled::flags_struct<CPUID1_EDX_FEATURE,
                           CPUID1_EDX_FEATURES>::flags_struct;
};

struct CPUID7_EBX_FEATURE final
    : sled::enum_struct<uint32_t, CPUID7_EBX_FEATURE> {
 public:
  static CPUID7_EBX_FEATURE const SGX;
  static CPUID7_EBX_FEATURE const HLE;
  static CPUID7_EBX_FEATURE const AVX2;
  static CPUID7_EBX_FEATURE const BMI2;
  static CPUID7_EBX_FEATURE const RTM;
  static CPUID7_EBX_FEATURE const AVX512f;
  static CPUID7_EBX_FEATURE const AVX512dq;
  static CPUID7_EBX_FEATURE const RDSEED;
  static CPUID7_EBX_FEATURE const AVX512pf;
  static CPUID7_EBX_FEATURE const AVX512er;
  static CPUID7_EBX_FEATURE const AVX512cd;
  static CPUID7_EBX_FEATURE const SHA;
  static CPUID7_EBX_FEATURE const AVX512bw;
  static CPUID7_EBX_FEATURE const AVX512vl;

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
};

constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::SGX{1 << 2};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::HLE{1 << 4};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX2{1 << 5};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::BMI2{1 << 8};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::RTM{1 << 11};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512f{1 << 16};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512dq{1 << 17};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::RDSEED{1 << 18};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512pf{1 << 26};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512er{1 << 27};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512cd{1 << 28};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::SHA{1 << 29};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512bw{1 << 30};
constexpr CPUID7_EBX_FEATURE const CPUID7_EBX_FEATURE::AVX512vl{1 << 31};

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

using CPUID1 =
    cpuid_call<1, uint32_t, uint32_t, CPUID1_ECX_FEATURES, CPUID1_EDX_FEATURES>;
using CPUID7 = cpuid_call<7, uint32_t, CPUID7_EBX_FEATURES, uint32_t, uint32_t>;

}  // namespace sled::x86
