/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"

namespace sled {

template <typename T, typename B>
struct typed_union {
  static_assert(std::is_trivially_constructible_v<T>);
  static_assert(std::is_default_constructible_v<T>);
  // XXX: static_assert(std::is_trivial_v<T>);
  using base_type = B;

  constexpr typed_union() = default;
  constexpr typed_union(base_type v) : v(v) {}
  constexpr typed_union(T b) : b(b) {}
  constexpr typed_union(const typed_union &rhs) = default;
  constexpr typed_union(typed_union &&rhs) = default;
  constexpr typed_union &operator=(const typed_union &that) = default;
  constexpr typed_union &operator=(typed_union &&that) = default;

  a_forceinline T &get() noexcept { return *safe_cast<T>(&v); }

  a_forceinline explicit operator T &() noexcept { return *safe_cast<T>(&v); }

  a_forceinline constexpr explicit operator T() const noexcept {
    return *safe_cast<T>(&v);
  }

  inline bool operator!=(const typed_union &rhs) const { return v != rhs.v; };
  inline bool operator==(const typed_union &rhs) const { return v == rhs.v; };

  // XXX: Workaround to avoid get() everywhere
  union {
    T b;
    base_type v;
  };
};

template <typename T, typename base_type, typename Tag>
struct strong_union {
  static_assert(std::is_trivially_constructible_v<T>);
  static_assert(std::is_default_constructible_v<T>);
  static_assert(sizeof(T) == sizeof(base_type));
  // XXX: static_assert(std::is_trivial_v<T>);

  constexpr strong_union() = default;
  constexpr strong_union(base_type v) : v(v) {}
  constexpr strong_union(T b) : b(b) {}
  constexpr strong_union(const strong_union &rhs) = default;
  constexpr strong_union(strong_union &&rhs) = default;
  constexpr strong_union &operator=(const strong_union &that) = default;
  constexpr strong_union &operator=(strong_union &&that) = default;

  a_forceinline T &get() noexcept { return *safe_cast<T>(&v); }

  a_forceinline explicit operator T &() noexcept { return *safe_cast<T>(&v); }

  a_forceinline constexpr explicit operator T() const noexcept {
    return *safe_cast<T>(&v);
  }

  inline bool operator!=(const strong_union &rhs) const { return v != rhs.v; };
  inline bool operator==(const strong_union &rhs) const { return v == rhs.v; };

  // XXX: Workaround to avoid get() everywhere
  union {
    T b;
    base_type v;
  };
};

}  // namespace sled
