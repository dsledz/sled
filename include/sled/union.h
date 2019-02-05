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
  template< typename Ta>
  constexpr typed_union(Ta v) : v(v) {}
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

template <typename T, typename B, typename Tag>
struct union_struct {
  static_assert(std::is_trivially_constructible_v<T>);
  static_assert(std::is_default_constructible_v<T>);
  static_assert(sizeof(T) == sizeof(B));
  // XXX: static_assert(std::is_trivial_v<T>);

  constexpr union_struct() = default;
  constexpr union_struct(B v) : v(v) {}
  constexpr union_struct(T b) : b(b) {}
  constexpr union_struct(const union_struct &rhs) = default;
  constexpr union_struct(union_struct &&rhs) = default;
  constexpr union_struct &operator=(const union_struct &that) = default;
  constexpr union_struct &operator=(union_struct &&that) = default;

  a_forceinline T &get() noexcept { return *safe_cast<T>(&v); }

  a_forceinline explicit operator T &() noexcept { return *safe_cast<T>(&v); }

  a_forceinline constexpr explicit operator T() const noexcept {
    return *safe_cast<T>(&v);
  }

  inline bool operator!=(const union_struct &rhs) const { return v != rhs.v; };
  inline bool operator==(const union_struct &rhs) const { return v == rhs.v; };

  inline Tag &operator=(const B &rhs) {
    Tag &t = static_cast<Tag &>(*this);
    t.b = rhs;
    return t;
  }

  // XXX: Workaround to avoid get() everywhere
  union {
    T b;
    B v;
  };
};

}  // namespace sled
