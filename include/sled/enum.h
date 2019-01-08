/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/exception.h"
#include "sled/platform.h"
#include "sled/string.h"

#include <ostream>
#include <type_traits>

namespace sled {

template <typename T, typename I>
static inline bool a_const enum_isset(I arg, T bit) {
  I n = static_cast<typename std::underlying_type<T>::type>(bit);
  return (arg & (1 << n));
}

template <typename T, typename R = uint8_t>
static inline R a_const enum_val(T t) {
  return static_cast<typename std::underlying_type<T>::type>(t);
}

template <typename T>
class Enum {
 public:
  class Iterator {
   public:
    explicit Iterator(int value) : m_value(value) {}

    T operator*() const { return T{m_value}; }

    void operator++() { ++m_value; }

    bool operator!=(Iterator rhs) { return m_value != rhs.m_value; }

   private:
    int m_value;
  };
};

template <typename T>
typename Enum<T>::Iterator begin(Enum<T> /*unused*/) {
  return typename Enum<T>::Iterator(enum_val(T::First));
}

template <typename T>
typename Enum<T>::Iterator end(Enum<T> /*unused*/) {
  return typename Enum<T>::Iterator(enum_val(T::Last) + 1);
}

template <typename T>
class BitField {
 public:
  using field_type = typename std::underlying_type<T>::type;

  BitField() = default;
  BitField(std::initializer_list<T> l) {
    for (auto b : l) {
      value_ |= static_cast<field_type>(b);
    }
  }
  explicit BitField(field_type value) : value_(value) {}

  bool empty() const { return value_ == 0; }

  void zero() { value_ = 0; }

  field_type &get() { return value_; }

  field_type get() const { return value_; }

  a_forceinline bool is_set(T t) const {
    return (value_ & static_cast<field_type>(t)) != 0;
  }

  a_forceinline bool is_clear(T t) const { return !is_set(t); }

  void clear(T t) { value_ &= ~static_cast<field_type>(t); }

  void set(T t) { value_ |= static_cast<field_type>(t); }

  void update(BitField to_set, BitField to_clear) {
    value_ = (value_ & ~to_clear.value_) | to_set.value_;
  }

  BitField<T> &operator|=(const BitField<T> &rhs) {
    value_ |= rhs.value_;
    return *this;
  }

  BitField<T> &operator|=(const T &rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  BitField<T> operator~() { return ~value_; }

  friend inline const BitField<T> operator|(const BitField<T> &lhs, T rhs) {
    BitField<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend inline constexpr BitField<T> operator|(BitField<T> const &lhs,
                                                BitField<T> const &rhs) {
    BitField<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend bool operator==(const BitField &lhs, const BitField &rhs) {
    return lhs.value_ == rhs.value_;
  }

 private:
  field_type value_{};
};

template <typename T, typename Tag>
struct StrongEnum {
 public:
  StrongEnum() = default;

  template <typename Ta>
  explicit constexpr StrongEnum(Ta v) : v(static_cast<T>(v)) {}

  T v{0};

  explicit constexpr operator T() const noexcept { return v; }

  a_forceinline constexpr explicit operator bool() const noexcept {
    return v != 0;
  }

  a_forceinline constexpr bool operator>(StrongEnum const &rhs) const noexcept {
    return v > rhs.v;
  }
  a_forceinline constexpr bool operator>=(StrongEnum const &rhs) const
      noexcept {
    return v >= rhs.v;
  }
  a_forceinline constexpr bool operator<(StrongEnum const &rhs) const noexcept {
    return v < rhs.v;
  }
  a_forceinline constexpr bool operator<=(StrongEnum const &rhs) const
      noexcept {
    return v <= rhs.v;
  }
  a_forceinline constexpr bool operator==(StrongEnum const &rhs) const
      noexcept {
    return v == rhs.v;
  }
  a_forceinline constexpr bool operator!=(StrongEnum const &rhs) const
      noexcept {
    return v != rhs.v;
  }

  friend inline std::ostream &operator<<(std::ostream &os,
                                         StrongEnum const &e) {
    auto &names = Tag::names;
    if (e.v >= names.size()) {
      os << "Invalid<" << e.v << ">";
    } else {
      os << names[e.v];
    }
    return os;
  }

  static inline StrongEnum from_string(std::string const &obj) {
    // TODO(dan): rework this function so it can be constexpr
    auto str = str_tolower(obj);
    for (size_t i = 0; i < Tag::names.size(); i++) {
      if (str == str_tolower(Tag::names[i])) {
        return StrongEnum{i};
      }
    }
    throw ConversionError(obj);
  }

  friend a_forceinline std::string to_string(StrongEnum const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

}  // namespace sled
