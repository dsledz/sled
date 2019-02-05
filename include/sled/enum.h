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

// TODO(dan): Is this required?
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
class bitfield {
 public:
  using field_type = typename std::underlying_type<T>::type;

  bitfield() = default;
  bitfield(std::initializer_list<T> l) {
    for (auto b : l) {
      value_ |= static_cast<field_type>(b);
    }
  }
  explicit bitfield(field_type value) : value_(value) {}

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

  void update(bitfield to_set, bitfield to_clear) {
    value_ = (value_ & ~to_clear.value_) | to_set.value_;
  }

  bitfield<T> &operator|=(const bitfield<T> &rhs) {
    value_ |= rhs.value_;
    return *this;
  }

  bitfield<T> &operator|=(const T &rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  bitfield<T> operator~() { return ~value_; }

  friend inline const bitfield<T> operator|(const bitfield<T> &lhs, T rhs) {
    bitfield<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend inline constexpr bitfield<T> operator|(bitfield<T> const &lhs,
                                                bitfield<T> const &rhs) {
    bitfield<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend bool operator==(const bitfield &lhs, const bitfield &rhs) {
    return lhs.value_ == rhs.value_;
  }

 private:
  field_type value_{};
};

template <typename T, typename Tag>
struct enum_struct {
 public:
  enum_struct() = default;

  using our_type = Tag;
  using field_type = T;
  using name_type = std::pair<T, const char *>;

  template <typename Ta>
  explicit constexpr enum_struct(Ta v) : v(static_cast<T>(v)) {}

  T v{0};

  constexpr operator T() const noexcept { return v; }

  a_forceinline constexpr explicit operator bool() const noexcept {
    return v != 0;
  }

  a_forceinline constexpr bool operator>(enum_struct const &rhs) const noexcept {
    return v > rhs.v;
  }
  a_forceinline constexpr bool operator>=(enum_struct const &rhs) const
      noexcept {
    return v >= rhs.v;
  }
  a_forceinline constexpr bool operator<(enum_struct const &rhs) const noexcept {
    return v < rhs.v;
  }
  a_forceinline constexpr bool operator<=(enum_struct const &rhs) const
      noexcept {
    return v <= rhs.v;
  }
  a_forceinline constexpr bool operator==(enum_struct const &rhs) const
      noexcept {
    return v == rhs.v;
  }
  a_forceinline constexpr bool operator!=(enum_struct const &rhs) const
      noexcept {
    return v != rhs.v;
  }

  friend inline std::ostream &operator<<(std::ostream &os,
                                         enum_struct const &e) {
    for (auto &name : our_type::names) {
      if (name.first == e.v) {
        os << name.second;
        return os;
      }
    }
    os << "Invalid<" << e.v << ">";
    return os;
  }

  static inline Tag from_string(std::string const &obj) {
    // TODO(dan): rework this function so it can be constexpr
    auto str = str_tolower(obj);
    for (auto &name : our_type::names) {
      if (str == str_tolower(name.second)) {
        return Tag{name.first};
      }
    }
    throw sled::ConversionError(obj);
  }

  friend a_forceinline std::string to_string(Tag const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

template <typename T, typename Tag>
class enum_bitfield {
 public:
  using field_type = typename T::field_type;

  enum_bitfield() = default;
  enum_bitfield(std::initializer_list<T> l) {
    for (auto b : l) {
      v |= b.v;
    }
  }
  explicit enum_bitfield(field_type value) : v(value) {}

  bool empty() const { return v == 0; }

  void zero() { v = 0; }

  field_type &get() { return v; }

  field_type get() const { return v; }

  a_forceinline bool is_set(T t) const { return (v & t.v) != 0; }

  a_forceinline bool is_clear(T t) const { return !is_set(t); }

  void clear(T t) { v &= ~t.v; }

  void set(T t) { v |= t.v; }

  void update(Tag to_set, Tag to_clear) {
    Tag &t = static_cast<Tag &>(*this);
    v = (v & ~to_clear.v) | to_set.v;
  }

  Tag &operator|=(Tag const &rhs) {
    Tag &t = static_cast<Tag &>(*this);
    t.v |= rhs.v;
    return t;
  }

  Tag &operator|=(const T &rhs) {
    Tag &t = static_cast<Tag &>(*this);
    t.v |= rhs.v;
    return t;
  }

  Tag operator~() { return ~v; }

  friend inline const Tag operator|(const Tag &lhs, T rhs) {
    Tag ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend inline constexpr Tag operator|(Tag const &lhs, Tag const &rhs) {
    Tag ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend bool operator==(Tag const &lhs, Tag const &rhs) {
    return lhs.v == rhs.v;
  }

  friend inline std::ostream &operator<<(std::ostream &os, Tag const &rhs) {
    bool first = true;
    for (auto &name : T::our_type::names) {
      if (__builtin_popcount(name.first) != 1) {
        continue;
      }
      if ((name.first & rhs.v) != 0) {
        if (first) {
          first = false;
        } else {
          os << " | ";
        }
        os << name.second;
      }
    }
    return os;
  }

  friend inline std::string to_string(Tag const &rhs) {
    std::stringstream ss;
    ss << rhs;
    return ss.str();
  }

 private:
  field_type v{};
};

}  // namespace sled
