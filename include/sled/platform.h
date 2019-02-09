/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <stdint.h>
#include <x86intrin.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>
#include <thread>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#include "sled/opts.h"

#ifdef WIN32
#define a_used
#define a_unused
#define a_const
#define a_forceinline __forceinline
#define a_noinline
#define likely(x) x
#define unlikely(x) x
#define __builtin_bswap64 _byteswap_uint64
#define linkas_real(x) __asm(#x)
#define linkas(x)
#else
#define a_used __attribute__((used))
#define a_unused __attribute((unused))
#define a_const __attribute__((const))
#define a_forceinline __attribute__((always_inline)) inline
#define a_noinline __attribute__((noinline))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define linkas_real(x) asm(#x)
#define linkas(x) linkas_real(x)

// Ensure a minimum alignment
#include <cstddef>
static_assert(alignof(std::max_align_t) == 16, "Unsupported alignment");
#endif

namespace sled {

#ifdef WIN32
struct stack_ctx {
  uint64_t rbx;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rsp;
  uint64_t rbp;
  uint64_t gs0;
  uint64_t gs8;
  uint64_t gs16;
};
#else
/**
 * Thread Context
 */
struct stack_ctx {
  uint64_t rsp; /**< Saved stack pointer. */
};
#endif

}  // namespace sled

#ifdef WIN32
// Windows building with support for throwing across extern C
#define linkas_real(x) __asm(#x)
#define linkas(x)
extern "C" {
#else
// Avoid marking the symbols extern C so we can throw across ASM.
#define linkas_real(x) asm(#x)
#define linkas(x) linkas_real(x)
#endif

/**
 * Switch Context.
 *
 * Switch the thread context, storing the current state in @a running_ctx and
 * resuming from @a new_ctx.
 */
extern void SwitchContext(sled::stack_ctx *running_ctx,
                          sled::stack_ctx *new_ctx) linkas(SwitchContext);

/**
 * Initial Switch Context.
 *
 * Switch the thread context, storing the current state in @a running_ctx and
 * resuming from @a new_ctx at @initial_ip
 */
extern void InitialSwitchContext(sled::stack_ctx *running_ctx,
                                 sled::stack_ctx *new_ctx, uintptr_t initial_ip)
    linkas(InitialSwitchContext);

/**
 * Switch Context with service routing.
 *
 * Switch the thread context, storing the current state in @a running_ctx and
 * resuming from @a new_ctx, but immediately executing @a isr_fn as
 * `void (*isr_fn)(isr_data);`
 */
extern void SwitchContextIrq(sled::stack_ctx *running_ctx,
                             sled::stack_ctx *new_ctx, uintptr_t isr_fn,
                             uintptr_t isr_data) linkas(SwitchContextIrq);

#ifdef WIN32
};
#endif

#ifndef NDEBUG
// XXX: assert usage should be replaced with runtime_assert()
#define debug_assert(...) assert(__VA_ARGS__)
#else
#define debug_assert(...)
#endif

namespace sled {

/**
 * Demagle support
 */
#ifdef WIN32

static inline std::string demangle(const std::type_info &ti) {
  return "unknown";
}

#else
#include <cxxabi.h>

static inline std::string demangle(const std::type_info &ti) {
  int status;
  auto name = abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status);
  std::string result(name);
  // NOLINTNEXTLINE
  free(name);
  return result;
}
#endif

}  // namespace sled

namespace sled {

template <typename T, typename B,
          class = typename std::enable_if_t<
              std::__and_<std::true_type /*std::is_trivially_copyable<T> */,
                          std::is_trivially_copyable<B>>::value>>
T *safe_cast(B *b) {
  std::aligned_storage<sizeof(T), alignof(T)> s;
  std::memcpy(&s, b, sizeof(T));
  auto f = new (b) T;
  std::memcpy(f, &s, sizeof(T));
  return f;
}

}  // namespace sled
