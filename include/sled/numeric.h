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
 * An Integer is a fixed width representation of an unsigned number.  Used as an
 * interchange and formating type.
 */
class Integer {
 public:
  template <typename T,
            class = typename std::enable_if_t<is_wrapped_integer<T>::value>>
  explicit constexpr Integer(T arg, T /*unused*/ = {})
      : v(arg.v), w(wrapped_integer_width(arg)) {}
  template <typename I,
            class = typename std::enable_if_t<std::is_integral<I>::value>>
  explicit constexpr Integer(I arg) : v(arg), w(sizeof(I) * 2) {}
  template <typename T,
            class = typename std::enable_if_t<std::is_enum<T>::value>>
  explicit constexpr Integer(
      T arg, typename std::underlying_type<T>::type /*unused*/ = 0)
      : v(static_cast<typename std::underlying_type<T>::type>(arg)),
        w(sizeof(typename std::underlying_type<T>) * 2) {}
  uint64_t v;
  int w;

  friend inline std::ostream &operator<<(std::ostream &os, Integer const &obj) {
    auto f = os.flags();
    os << std::dec << obj.v;
    os.flags(f);
    return os;
  }

  static inline Integer from_string(std::string const &obj) {
    // XXX: Pick the correct width.
    return Integer(stoull(obj, nullptr, 0));
  }

  friend a_forceinline std::string to_string(Integer const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

template <class T>
inline T from_integer(const Integer &integer) {
  if constexpr (std::is_integral<T>::value || std::is_enum<T>::value) {
    return static_cast<T>(integer.v);
  } else if constexpr (sled::is_wrapped_integer<T>::value) {
    return T(integer.v);
  }
  // XXX: This should be a compile time error.
  throw sled::ConversionError("integer", "integer conversion not supported");
}

/**
 * Integer Integer formatting flags.
 */
enum class IntegerFormat {
  KiB = 0x01,
  MiB = 0x02,
};

/**
 * Alternative representation of a hex number, e.g. $BEEF.
 */
class AltInteger {
 public:
  using Flags = sled::flags<IntegerFormat>;
  template <typename T,
            class = typename std::enable_if<is_wrapped_integer<T>::value>::type>
  explicit constexpr AltInteger(T arg, Flags flags = Flags{}, T /*unused*/ = {})
      : v(arg.v), w(wrapped_integer_width(arg)), flags{flags} {}
  template <typename I,
            class = typename std::enable_if<std::is_integral<I>::value>::type>
  explicit constexpr AltInteger(I arg, Flags flags = Flags{})
      : v(arg), w(sizeof(I) * 2), flags{flags} {}
  template <typename T,
            class = typename std::enable_if<std::is_enum<T>::value>::type>
  explicit constexpr AltInteger(
      T arg, Flags flags = Flags{},
      typename std::underlying_type<T>::type /*unused*/ = 0)
      : v(static_cast<typename std::underlying_type<T>::type>(arg)),
        w(sizeof(typename std::underlying_type<T>) * 2),
        flags{flags} {}
  explicit constexpr AltInteger(Integer i) : v(i.v), w(i.w) {}
  uint64_t v;
  int w;
  Flags flags{};

  friend inline std::ostream &operator<<(std::ostream &os,
                                         AltInteger const &obj) {
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

  static inline AltInteger from_string(std::string const &obj) {
    // XXX: Pick the correct width.
    return AltInteger(stoull(obj, nullptr, 0));
  }

  friend a_forceinline std::string to_string(AltInteger const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

/**
 * Hex Integer formatting flags.
 */
enum class HexFormat {
  NoPrefix = 0x01,
  NoPadding = 0x02,
};

/**
 * Standard representation of a hex number, 0xBEEF
 */
class Hex {
 public:
  template <typename T,
            class = typename std::enable_if<is_wrapped_integer<T>::value>::type>
  explicit constexpr Hex(T arg, T /*unused*/ = {})
      : v(arg.v), w(wrapped_integer_width(arg)) {}
  template <typename I,
            class = typename std::enable_if<std::is_integral<I>::value>::type>
  explicit constexpr Hex(I arg) : v(arg), w(sizeof(I) * 2) {}
  template <typename T,
            class = typename std::enable_if<std::is_enum<T>::value>::type>
  explicit constexpr Hex(T arg,
                         typename std::underlying_type<T>::type /*unused*/ = 0)
      : v(static_cast<typename std::underlying_type<T>::type>(arg)),
        w(sizeof(typename std::underlying_type<T>) * 2) {}
  explicit constexpr Hex(std::byte b) : v(std::to_integer<uint64_t>(b)), w(2) {}
  explicit constexpr Hex(Integer i) : v(i.v), w(i.w) {}
  uint64_t v;
  int w;
  int flags{0};

  constexpr bool operator==(Hex const &rhs) { return v == rhs.v; }

  constexpr bool operator!=(Hex const &rhs) { return v == rhs.v; }

  friend inline std::ostream &operator<<(std::ostream &os, Hex const &obj) {
    auto f = os.flags();
    if (obj.flags == 0) {
      os << "0x";
    }
    os << std::hex << std::setfill('0') << std::right << std::setw(obj.w)
       << obj.v << std::dec;
    os.flags(f);
    return os;
  }

  friend inline bool operator==(const Hex &lhs, const Hex &rhs) {
    return lhs.v == rhs.v && lhs.w == rhs.w;
  }

  static inline Hex from_string(std::string const &obj) {
    // XXX: Pick the correct width.
    return Hex(stoull(obj, nullptr, 0));
  }

  friend a_forceinline std::string to_string(Hex const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

/**
 * Alternative representation of a hex number, e.g. $BEEF.
 */
class AltHex {
 public:
  using Flags = sled::flags<HexFormat>;
  template <typename T,
            class = typename std::enable_if<is_wrapped_integer<T>::value>::type>
  explicit constexpr AltHex(T arg, Flags flags = Flags{}, T /*unused*/ = {})
      : v(arg.v), w(wrapped_integer_width(arg)), flags{flags} {}
  template <typename I,
            class = typename std::enable_if<std::is_integral<I>::value>::type>
  explicit constexpr AltHex(I arg, Flags flags = Flags{})
      : v(arg), w(sizeof(I) * 2), flags{flags} {}
  template <typename T,
            class = typename std::enable_if<std::is_enum<T>::value>::type>
  explicit constexpr AltHex(
      T arg, Flags flags = Flags{},
      typename std::underlying_type<T>::type /*unused*/ = 0)
      : v(static_cast<typename std::underlying_type<T>::type>(arg)),
        w(sizeof(typename std::underlying_type<T>) * 2),
        flags{flags} {}
  explicit constexpr AltHex(std::byte b, Flags flags = Flags{})
      : v(std::to_integer<uint64_t>(b)), w(2), flags{flags} {}
  explicit constexpr AltHex(Integer i) : v(i.v), w(i.w) {}
  uint64_t v;
  int w;
  Flags flags{};

  friend inline std::ostream &operator<<(std::ostream &os, AltHex const &obj) {
    auto f = os.flags();
    if (obj.flags.is_clear(HexFormat::NoPrefix)) {
      os << "$";
    }
    if (obj.flags.is_clear(HexFormat::NoPadding)) {
      os << std::setfill('0');
      os << std::setw(obj.w);
    }
    os << std::hex << std::uppercase << std::right << obj.v;
    os.flags(f);
    return os;
  }

  static inline AltHex from_string(std::string const &obj) {
    // XXX: Pick the correct width.
    return AltHex(stoull(obj, nullptr, 0));
  }

  friend a_forceinline std::string to_string(AltHex const &obj) {
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
