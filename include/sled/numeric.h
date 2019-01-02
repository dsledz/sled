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
};

/**
 * Hex Integer formatting flags.
 */
enum class HexFormat {
  NoPrefix = 0x01,
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
  explicit constexpr Hex(Integer i) : v(i.v), w(i.w) {}
  uint64_t v;
  int w;
  int flags{0};

  constexpr bool operator==(Hex const &rhs) { return v == rhs.v; }

  constexpr bool operator!=(Hex const &rhs) { return v == rhs.v; }

  friend inline std::ostream &operator<<(std::ostream &os, Hex const &obj) {
    if (obj.flags == 0) {
      os << "0x";
    }
    os << std::hex << std::setfill('0') << std::right << std::setw(obj.w)
       << obj.v << std::dec;
    return os;
  }
};

/**
 * Alternative representation of a hex number, e.g. $BEEF.
 */
class AltHex {
 public:
  using Flags = BitField<HexFormat>;
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
  explicit constexpr AltHex(Integer i) : v(i.v), w(i.w) {}
  uint64_t v;
  int w;
  Flags flags{};

  friend inline std::ostream &operator<<(std::ostream &os, AltHex const &obj) {
    if (obj.flags.is_clear(HexFormat::NoPrefix)) {
      os << "$";
    }
    os << std::hex << std::uppercase << std::setfill('0') << std::right
       << std::setw(obj.w) << obj.v;
    return os;
  }
};

};  // namespace sled
