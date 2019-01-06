/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#pragma once

#include "sled/numeric.h"
#include "sled/platform.h"

#include <ostream>

namespace sled {

/**
 * Strong Integer template.
 *
 * Strong Integers are used when it's desirable to have a strong type that can
 * be represented by an integral value but you want to restrict operations
 * between other types.
 */
template <typename T, typename Tag>
class StrongInteger {
 public:
  StrongInteger() = default;
  template <typename Ta>
  explicit constexpr StrongInteger(Ta v) : v(static_cast<T>(v)) {}

  T v{0};

  explicit constexpr operator T() const noexcept { return v; }

  constexpr T &get() noexcept { return v; }

  constexpr T get() const noexcept { return v; }

  a_forceinline constexpr StrongInteger operator+(
      StrongInteger const &rhs) const noexcept {
    return StrongInteger(v + rhs.v);
  }

  a_forceinline constexpr StrongInteger operator-(
      StrongInteger const &rhs) const noexcept {
    return StrongInteger(v - rhs.v);
  }

  a_forceinline constexpr StrongInteger operator*(
      StrongInteger const &rhs) const noexcept {
    return StrongInteger(v * rhs.v);
  }

  a_forceinline constexpr StrongInteger operator/(
      StrongInteger const &rhs) const noexcept {
    return StrongInteger(v / rhs.v);
  }

  a_forceinline constexpr StrongInteger operator%(
      StrongInteger const &rhs) const noexcept {
    return StrongInteger(v % rhs.v);
  }

  a_forceinline constexpr StrongInteger operator+=(
      StrongInteger const &rhs) noexcept {
    v += rhs.v;
    return *this;
  }

  a_forceinline constexpr StrongInteger operator-=(
      StrongInteger const &rhs) noexcept {
    v -= rhs.v;
    return *this;
  }

  a_forceinline constexpr StrongInteger operator*=(
      StrongInteger const &rhs) noexcept {
    v *= rhs.v;
    return *this;
  }

  a_forceinline constexpr StrongInteger operator/=(
      StrongInteger const &rhs) noexcept {
    v /= rhs.v;
    return *this;
  }

  a_forceinline constexpr StrongInteger operator%=(
      StrongInteger const &rhs) noexcept {
    v %= rhs.v;
    return *this;
  }

  a_forceinline constexpr explicit operator bool() const noexcept {
    return v != 0;
  }

  a_forceinline constexpr bool operator>(StrongInteger const &rhs) const
      noexcept {
    return v > rhs.v;
  }
  a_forceinline constexpr bool operator>=(StrongInteger const &rhs) const
      noexcept {
    return v >= rhs.v;
  }
  a_forceinline constexpr bool operator<(StrongInteger const &rhs) const
      noexcept {
    return v < rhs.v;
  }
  a_forceinline constexpr bool operator<=(StrongInteger const &rhs) const
      noexcept {
    return v <= rhs.v;
  }
  a_forceinline constexpr bool operator==(StrongInteger const &rhs) const
      noexcept {
    return v == rhs.v;
  }
  a_forceinline constexpr bool operator!=(StrongInteger const &rhs) const
      noexcept {
    return v != rhs.v;
  }
  static a_forceinline constexpr StrongInteger minimum(
      StrongInteger const &a) noexcept {
    return a;
  }
  static a_forceinline StrongInteger minimum(StrongInteger const &a,
                                             StrongInteger const &b) noexcept {
    return a < b ? a : b;
  }
  template <typename... Ta>
  static a_forceinline StrongInteger minimum(StrongInteger const &a,
                                             StrongInteger const &b,
                                             Ta const &... t) noexcept {
    return minimum(a, minimum(b, t...));
  }
  static a_forceinline constexpr StrongInteger maximum(
      StrongInteger const &a) noexcept {
    return a;
  }
  static a_forceinline StrongInteger maximum(StrongInteger const &a,
                                             StrongInteger const &b) noexcept {
    return a > b ? a : b;
  }
  template <typename... Ta>
  static a_forceinline StrongInteger maximum(StrongInteger const &a,
                                             StrongInteger const &b,
                                             Ta const &... t) noexcept {
    return maximum(a, maximum(b, t...));
  }

  friend std::ostream &operator<<(std::ostream &os, StrongInteger const &rhs) {
    os << static_cast<T>(rhs.v);
    return os;
  }
};

template <typename T, typename Tag>
struct __is_wrapped_integer_helper<StrongInteger<T, Tag>>
    : public std::true_type {};

/**
 * Strong Int template.
 *
 * Example usage:
 * struct DomainInt : StrongInt<uint32_t, DomainInt> { }
 *
 * Strong Integers are used when it's desirable to have a strong type that can
 * be represented by an integral value but you want to restrict operations
 * between other types.
 */
template <typename T, typename Tag>
class StrongInt {
 public:
  StrongInt() = default;
  template <typename Ta>
  explicit constexpr StrongInt(Ta v) : v(static_cast<T>(v)) {}

  T v{0};

  explicit constexpr operator T() const noexcept { return v; }

  constexpr T &get() noexcept { return v; }

  constexpr T get() const noexcept { return v; }

  a_forceinline constexpr Tag operator+(Tag const &rhs) const noexcept {
    return Tag(v + rhs.v);
  }

  a_forceinline constexpr Tag operator-(Tag const &rhs) const noexcept {
    return Tag(v - rhs.v);
  }

  a_forceinline constexpr Tag operator*(Tag const &rhs) const noexcept {
    return Tag(v * rhs.v);
  }

  a_forceinline constexpr Tag operator/(Tag const &rhs) const noexcept {
    return Tag(v / rhs.v);
  }

  a_forceinline constexpr Tag operator%(Tag const &rhs) const noexcept {
    return Tag(v % rhs.v);
  }

  a_forceinline constexpr Tag operator+=(Tag const &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v += rhs.v;
    return t;
  }

  a_forceinline constexpr Tag operator-=(Tag const &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v -= rhs.v;
    return t;
  }

  a_forceinline constexpr Tag operator*=(Tag const &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v *= rhs.v;
    return t;
  }

  a_forceinline constexpr Tag operator/=(Tag const &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v /= rhs.v;
    return t;
  }

  a_forceinline constexpr Tag operator%=(Tag const &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v %= rhs.v;
    return t;
  }

  a_forceinline constexpr explicit operator bool() const noexcept {
    return v != 0;
  }

  a_forceinline constexpr bool operator>(Tag const &rhs) const noexcept {
    return v > rhs.v;
  }
  a_forceinline constexpr bool operator>=(Tag const &rhs) const noexcept {
    return v >= rhs.v;
  }
  a_forceinline constexpr bool operator<(Tag const &rhs) const noexcept {
    return v < rhs.v;
  }
  a_forceinline constexpr bool operator<=(Tag const &rhs) const noexcept {
    return v <= rhs.v;
  }
  a_forceinline constexpr bool operator==(Tag const &rhs) const noexcept {
    return v == rhs.v;
  }
  a_forceinline constexpr bool operator!=(Tag const &rhs) const noexcept {
    return v != rhs.v;
  }
  static a_forceinline constexpr Tag minimum(Tag const &a) noexcept {
    return a;
  }
  static a_forceinline Tag minimum(Tag const &a, Tag const &b) noexcept {
    return a < b ? a : b;
  }
  template <typename... Ta>
  static a_forceinline Tag minimum(Tag const &a, Tag const &b,
                                   Ta const &... t) noexcept {
    return minimum(a, minimum(b, t...));
  }
  static a_forceinline constexpr Tag maximum(Tag const &a) noexcept {
    return a;
  }
  static a_forceinline Tag maximum(Tag const &a, Tag const &b) noexcept {
    return a > b ? a : b;
  }
  template <typename... Ta>
  static a_forceinline Tag maximum(Tag const &a, Tag const &b,
                                   Ta const &... t) noexcept {
    return maximum(a, maximum(b, t...));
  }
};

};  // namespace sled

namespace std {
template <class T, typename Tag>
class hash<sled::StrongInteger<T, Tag>> {
 public:
  size_t operator()(sled::StrongInteger<T, Tag> const &x) const {
    return std::hash<T>()(x.v);
  }
};
}  // namespace std
