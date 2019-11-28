/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <type_traits>

#include "sled/enum.h"

namespace sled {

template <typename>
struct __is_wrapped_integer_helper : public std::false_type {};

/**
 * Specialization point to indicate the type is a wrapped integer, with a field
 * @a v that contains the value.
 */
template <typename _Tp>
struct is_wrapped_integer : public __is_wrapped_integer_helper<
                                typename std::remove_cv<_Tp>::type>::type {};

/**
 * Specialization point to return the fixed width of characters for a wrapped
 * integer.
 */
template <typename _Tp,
          class = typename std::enable_if_t<is_wrapped_integer<_Tp>::value>>
constexpr int wrapped_integer_width(_Tp const &arg) {
  return sizeof(arg.v) * 2;
}

/**
 * 4-Bit number
 */
struct nibble_t {
  nibble_t() = default;
  explicit constexpr nibble_t(uint8_t v) : v(v) {}
  uint8_t v{0};
};

template <>
struct __is_wrapped_integer_helper<nibble_t> : public std::true_type {};

template <>
constexpr int wrapped_integer_width<nibble_t>(nibble_t const &arg) {
  return 1;
}

/**
 * IntFmt IntFmt formatting flags.
 */
enum class IntegerFormat {
  KiB = 0x01,
  MiB = 0x02,
};

/**
 * Alternative representation of a hex number, e.g. $BEEF.
 */
class IntFmt {
 public:
  using Flags = sled::flags<IntegerFormat>;
  template <typename T,
            class = typename std::enable_if<is_wrapped_integer<T>::value>::type>
  explicit constexpr IntFmt(T arg, Flags flags = Flags{}, T /*unused*/ = {})
      : v(arg.v), w(wrapped_integer_width(arg)), flags{flags} {}
  template <typename I,
            class = typename std::enable_if<std::is_integral<I>::value>::type>
  explicit constexpr IntFmt(I arg, Flags flags = Flags{})
      : v(arg), w(sizeof(I) * 2), flags{flags} {}
  template <typename T,
            class = typename std::enable_if<std::is_enum<T>::value>::type>
  explicit constexpr IntFmt(
      T arg, Flags flags = Flags{},
      typename std::underlying_type<T>::type /*unused*/ = 0)
      : v(static_cast<typename std::underlying_type<T>::type>(arg)),
        w(sizeof(typename std::underlying_type<T>) * 2),
        flags{flags} {}
  uint64_t v;
  int w;
  Flags flags{};

  friend inline std::ostream &operator<<(std::ostream &os, IntFmt const &obj) {
    auto f = os.flags();
    if (obj.flags.is_set(IntegerFormat::KiB)) {
      static constexpr uint64_t KiB_size = 1024;
      int64_t quot = obj.v / KiB_size;
      int32_t rem = std::abs(static_cast<int32_t>(obj.v % KiB_size));
      os << std::dec << quot;
      if (rem) {
        rem = (10 * rem) / KiB_size;
        os << "." << rem;
      }
      os << "KiB";
    } else if (obj.flags.is_set(IntegerFormat::MiB)) {
      static constexpr uint64_t MiB_size = 1024 * 1024;
      int64_t quot = obj.v / MiB_size;
      int64_t rem = std::abs(static_cast<int64_t>(obj.v % MiB_size));
      os << std::dec << quot;
      if (rem) {
        rem = (10 * rem) / MiB_size;
        os << "." << rem;
      }
      os << "MiB";
    } else {
      os << std::dec << obj.v;
    }
    os.flags(f);
    return os;
  }

  static inline IntFmt from_string(std::string const &obj) {
    // XXX: Pick the correct width.
    return IntFmt(stoull(obj, nullptr, 0));
  }

  friend a_forceinline std::string fmt_string(IntFmt const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

template <class T>
inline T from_integer(const IntFmt &integer) {
  if constexpr (std::is_integral<T>::value || std::is_enum<T>::value) {
    return static_cast<T>(integer.v);
  } else if constexpr (sled::is_wrapped_integer<T>::value) {
    return T(integer.v);
  }
  // XXX: This should be a compile time error.
  throw sled::ConversionError("integer", "integer conversion not supported");
}

/**
 * HexFmt formatting flags.
 */
enum class HexFormat {
  AltPrefix = 0x01,
  NoPrefix = 0x02,
  NoPadding = 0x04,
};

/**
 * Standard representation of a number in hex.
 */
class HexFmt {
 public:
  using Flags = sled::flags<HexFormat>;
  template <typename T,
            class = typename std::enable_if<is_wrapped_integer<T>::value>::type>
  explicit constexpr HexFmt(T arg, Flags flags = Flags{}, T /*unused*/ = {})
      : v(arg.v), w(wrapped_integer_width(arg)), flags{flags} {}
  template <typename I,
            class = typename std::enable_if<std::is_integral<I>::value>::type>
  explicit constexpr HexFmt(I arg, Flags flags = Flags{})
      : v(arg), w(sizeof(I) * 2), flags{flags} {}
  template <typename T,
            class = typename std::enable_if<std::is_enum<T>::value>::type>
  explicit constexpr HexFmt(
      T arg, Flags flags = Flags{},
      typename std::underlying_type<T>::type /*unused*/ = 0)
      : v(static_cast<typename std::underlying_type<T>::type>(arg)),
        w(sizeof(typename std::underlying_type<T>) * 2),
        flags{flags} {}
  explicit constexpr HexFmt(std::byte b, Flags flags = Flags{})
      : v(std::to_integer<uint64_t>(b)), w(2), flags{flags} {}
  explicit constexpr HexFmt(IntFmt i) : v(i.v), w(i.w) {}
  uint64_t v;
  int w;
  Flags flags{0};

  constexpr bool operator==(HexFmt const &rhs) { return v == rhs.v; }

  constexpr bool operator!=(HexFmt const &rhs) { return v == rhs.v; }

  friend inline std::ostream &operator<<(std::ostream &os, HexFmt const &obj) {
    auto f = os.flags();
    if (obj.flags.is_clear(HexFormat::NoPrefix)) {
      if (obj.flags.is_clear(HexFormat::AltPrefix)) {
        os << "0x";
      } else {
        os << "$";
      }
    }
    if (obj.flags.is_clear(HexFormat::NoPadding)) {
      os << std::setfill('0');
      os << std::right;
      os << std::setw(obj.w);
    }
    os << std::hex << std::uppercase << std::right << obj.v;
    os.flags(f);
    return os;
  }

  friend inline bool operator==(const HexFmt &lhs, const HexFmt &rhs) {
    return lhs.v == rhs.v && lhs.w == rhs.w;
  }

  static inline HexFmt from_string(std::string const &obj) {
    // XXX: Pick the correct width.
    return HexFmt(stoull(obj, nullptr, 0));
  }

  friend a_forceinline std::string fmt_string(HexFmt const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

/**
 * Helper to explicitly narrow
 */
struct u8 {
  constexpr explicit u8(int v) : v(static_cast<uint8_t>(v)) {}
  constexpr inline operator uint8_t() const { return v; }
  uint8_t v{};
};

template <>
struct __is_wrapped_integer_helper<u8> : public std::true_type {};

/**
 * Saturated u8 cast
 */
struct u8s {
  constexpr explicit u8s(int v)
      : v(static_cast<uint8_t>(std::min(
            v, static_cast<int>((std::numeric_limits<uint8_t>::max)())))) {}
  constexpr inline operator uint8_t() const { return v; }
  uint8_t v{};
};

template <>
struct __is_wrapped_integer_helper<u8s> : public std::true_type {};

#if 0
/*  ____  _ _      ___
 * | __ )(_) |_   / _ \ _ __  ___
 * |  _ \| | __| | | | | '_ \/ __|
 * | |_) | | |_  | |_| | |_) \__ \
 * |____/|_|\__|  \___/| .__/|___/
 *                     |_|
 */
template <typename A, typename T,
          class = typename std::enable_if_t<std::is_enum_v<T> > >
static inline A bit_set(A arg, T bit, bool val) {
  auto n = static_cast<typename std::underlying_type<T>::type>(bit);
  arg &= ~(1u << n);
  arg |= (val ? (1u << n) : 0);
  return arg;
}

template <typename A>
constexpr static inline A bit_set(A arg, unsigned n, bool val) {
  arg &= ~(1u << n);
  arg |= (val ? (1u << n) : 0);
  return arg;
}

constexpr static inline uint8_t bit_setmask(uint8_t arg, uint8_t mask, uint8_t val) {
  return (arg & ~mask) | val;
}

template <typename A>
constexpr static a_forceinline bool bit_isset(A arg, int bit) {
  return (((arg) & (1u << (bit))) != 0);
}

template <typename A>
constexpr static a_forceinline A bit_toggle(A arg1, A arg2, int bit) {
  return ((bit_isset((arg1), (bit)) ^ bit_isset((arg2), (bit))) &&
   bit_isset((arg1), (bit)));
}

#endif

};  // namespace sled
