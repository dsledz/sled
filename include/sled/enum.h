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

/**
 * Wrapper for flags based upon an "enum class"
 */
template <typename T>
class flags {
 public:
  using field_type = typename std::underlying_type<T>::type;

  flags() = default;
  flags(std::initializer_list<T> l) {
    for (auto b : l) {
      value_ |= static_cast<field_type>(b);
    }
  }
  explicit flags(field_type value) : value_(value) {}

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

  void update(flags to_set, flags to_clear) {
    value_ = (value_ & ~to_clear.value_) | to_set.value_;
  }

  flags<T> &operator|=(const flags<T> &rhs) {
    value_ |= rhs.value_;
    return *this;
  }

  flags<T> &operator|=(const T &rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  flags<T> operator~() { return ~value_; }

  friend inline const flags<T> operator|(const flags<T> &lhs, T rhs) {
    flags<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend inline constexpr flags<T> operator|(flags<T> const &lhs,
                                             flags<T> const &rhs) {
    flags<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend bool operator==(const flags &lhs, const flags &rhs) {
    return lhs.value_ == rhs.value_;
  }

 private:
  field_type value_{};
};

template <typename T, typename Tag>
struct enum_struct {
 public:
  using our_type = Tag;
  using field_type = T;
  using name_type = std::pair<T, const char *>;

 public:
  class forward_iterator {
   public:
    constexpr explicit forward_iterator() : i_(0) {}
    explicit forward_iterator(int i) : i_(i) {}
    explicit forward_iterator(Tag &v) : i_(0) {
      while (i_ < our_type::names.size()) {
        if (our_type::names[i_].first == v.v) {
          break;
        }
      }
    }

    Tag operator*() const { return Tag{our_type::names[i_].first}; }

    void operator++() { ++i_; }

    bool operator!=(forward_iterator rhs) { return i_ != rhs.i_; }

   private:
    int i_;
  };

  friend forward_iterator begin(enum_struct /*unused*/) {
    return forward_iterator();
  }

  friend forward_iterator end(enum_struct /*unused*/) {
    return forward_iterator(our_type::names.size());
  }

 public:
  constexpr enum_struct() = default;
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
    os << "Invalid<" << (int)e.v << ">";
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

/**
 * Flags based on an enum.
 */
template <typename T, typename Tag>
class flags_struct {
 public:
  using field_type = typename T::field_type;

  flags_struct() = default;
  flags_struct(std::initializer_list<T> l) {
    for (auto b : l) {
      v |= b.v;
    }
  }
  explicit flags_struct(field_type value) : v(value) {}

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
