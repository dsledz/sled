/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/exception.h"
#include "sled/platform.h"
#include "sled/string.h"

namespace sled {

/**
 * Flags class to wrap an "enum class" to work more like a normal enum
 * but with enhanced type-checking.
 *
 * Designed to be a zero-cost abstraction.
 */
template <typename T>
class flags {
 public:
  using field_type = typename std::underlying_type<T>::type;

  constexpr flags() = default;
  constexpr flags(std::initializer_list<T> l) {
    for (auto b : l) {
      value_ |= static_cast<field_type>(b);
    }
  }
  explicit constexpr flags(field_type value) : value_(value) {}

  a_forceinline bool empty() const noexcept { return value_ == 0; }

  a_forceinline void zero() noexcept { value_ = 0; }

  a_forceinline field_type &get() noexcept { return value_; }

  a_forceinline field_type get() const noexcept { return value_; }

  a_forceinline bool is_set(T t) const noexcept {
    return (value_ & static_cast<field_type>(t)) != 0;
  }

  a_forceinline bool is_clear(T t) const noexcept { return !is_set(t); }

  a_forceinline void clear(T t) noexcept {
    value_ &= ~static_cast<field_type>(t);
  }

  a_forceinline void set(T t) noexcept { value_ |= static_cast<field_type>(t); }

  a_forceinline void update(flags to_set, flags to_clear) noexcept {
    value_ = (value_ & ~to_clear.value_) | to_set.value_;
  }

  a_forceinline flags<T> &operator|=(const flags<T> &rhs) noexcept {
    value_ |= rhs.value_;
    return *this;
  }

  a_forceinline flags<T> &operator|=(const T &rhs) noexcept {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  a_forceinline flags<T> operator~() noexcept { return ~value_; }

  friend inline const flags<T> operator|(const flags<T> &lhs, T rhs) noexcept {
    flags<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend inline constexpr flags<T> operator|(flags<T> const &lhs,
                                             flags<T> const &rhs) noexcept {
    flags<T> ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend bool operator==(const flags &lhs, const flags &rhs) noexcept {
    return lhs.value_ == rhs.value_;
  }

 private:
  field_type value_{};
};

/**
 * A more complex introspection-based base class for an enumeration.
 *
 * This class should be subclassed before use.
 * Members @names and @V should be defined as in the example.
 * The awkwardness exists to allow constexpr usage of structure.
 *
 * @a T - Integral type suitable to hold all possible values of the enumeration.
 * @a Tag - Enumeration type.
 *
 * \code{.cpp}
 * struct Enum final : sled::enum_struct<uint32_t, Enum> {
 *   public:
 *     static constexpr std::array<name_type, 1> names{
 *       std::make_pair(1, "Val")};
 *     using sled::enum_struct<uint32_t, EnumValues>::enum_struct;
 *     struct V;
 * };
 *
 * struct Enum::V {
 *    static constexpr Enum Val{1};
 * };
 * \endcode{}
 */
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
    return forward_iterator(static_cast<T>(our_type::names.size()));
  }

 public:
  constexpr enum_struct() = default;
  template <typename Ta>
  explicit constexpr enum_struct(Ta v) : v(static_cast<T>(v)) {}

  T v{0};

  constexpr operator T() const noexcept { return v; }

#if 0
  a_forceinline constexpr explicit operator bool() const noexcept {
    return v != 0;
  }
#endif

  a_forceinline friend constexpr bool operator>(
      enum_struct const &lhs, enum_struct const &rhs) noexcept {
    return lhs.v > rhs.v;
  }
  a_forceinline constexpr bool operator>=(enum_struct const &rhs) const
      noexcept {
    return v >= rhs.v;
  }
  a_forceinline friend constexpr bool operator<(
      enum_struct const &lhs, enum_struct const &rhs) noexcept {
    return lhs.v < rhs.v;
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
    for (auto &name : our_type::names) {
      if (str_iequal(obj, name.second)) {
        return Tag{name.first};
      }
    }
    throw sled::ConversionError(obj);
  }

  static inline std::ostream &choices(std::ostream &os) {
    bool first = true;
    os << "[";
    for (auto &name : our_type::names) {
      if (first) {
        first = false;
      } else {
        os << ", ";
      }
      os << name.second;
    }
    os << "]";
    return os;
  }

  friend a_forceinline std::string to_string(Tag const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }

  /**
   * TODO: This should be in EnumFmt
   */
  friend a_forceinline std::string fmt_read(Tag const &obj) {
    std::stringstream os;
    os << obj;
    return os.str();
  }
};

/**
 * An introspection-based base class for a set of flags.
 *
 * This class should be subclassed before use.
 * Members @names and @V should be defined as in the example.
 * The awkwardness exists to allow constexpr usage.
 *
 * @a T - A subclassed enum_struct<>
 * @a Tag - flags type.
 *
 * Flags based on an enum.
 *
 * \code{.cpp}
 * struct Enum final : sled::enum_struct<uint32_t, Enum>{};
 * struct Flags final : sled::flags_struct<Enum, Flags> {
 *   using flags_struct::flags_struct;
 * }
 * \endcode{}
 */
template <typename T, typename Tag>
class flags_struct {
 public:
  using field_type = typename T::field_type;

  constexpr flags_struct() = default;
  constexpr flags_struct(std::initializer_list<T> l) {
    for (auto b : l) {
      v |= b.v;
    }
  }
  explicit constexpr flags_struct(field_type value) : v(value) {}

  bool empty() const noexcept { return v == 0; }

  void zero() noexcept { v = 0; }

  a_forceinline field_type &get() noexcept { return v; }

  a_forceinline field_type get() const noexcept { return v; }

  a_forceinline bool is_set(T t) const noexcept { return (v & t.v) != 0; }

  a_forceinline bool is_clear(T t) const noexcept { return !is_set(t); }

  a_forceinline void clear(T t) noexcept { v &= ~t.v; }

  a_forceinline void set(T t) noexcept { v |= t.v; }

  a_forceinline void update(Tag to_set, Tag to_clear) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    v = (v & ~to_clear.v) | to_set.v;
  }

  a_forceinline Tag &operator|=(Tag const &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v |= rhs.v;
    return t;
  }

  a_forceinline Tag &operator|=(const T &rhs) noexcept {
    Tag &t = static_cast<Tag &>(*this);
    t.v |= rhs.v;
    return t;
  }

  a_forceinline Tag operator~() const noexcept { return ~v; }

  friend inline const Tag operator|(const Tag &lhs, T rhs) noexcept {
    Tag ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend inline constexpr Tag operator|(Tag const &lhs,
                                        Tag const &rhs) noexcept {
    Tag ret(lhs);
    ret |= rhs;
    return ret;
  }

  friend bool operator==(Tag const &lhs, Tag const &rhs) noexcept {
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

  friend inline std::string fmt_read(Tag const &rhs) {
    std::stringstream ss;
    ss << rhs;
    return ss.str();
  }

  static inline Tag from_string(std::string const &obj) {
    Tag value{};
    int last = 0;
    int off = 0;
    do {
      off = obj.find("|", last);
      std::string v = [&]() {
        if (off == std::string::npos) {
          return obj.substr(last);
        } else {
          return obj.substr(last, off - last);
        }
      }();
      sled::trim(v);
      if (auto it = std::find_if(
              T::our_type::names.begin(), T::our_type::names.end(),
              [&](auto &it) { return sled::str_iequal(it.second, v); });
          it != T::our_type::names.end()) {
        value |= Tag(it->first);
      } else if (!v.empty()) {
        throw sled::ConversionError("Flags", obj);
      }
      last = off + 1;
    } while (off != std::string::npos);
    return value;
  }

  static inline std::ostream &choices(std::ostream &os) {
    bool first = true;
    os << "[";
    for (auto &name : T::our_type::names) {
      if (first) {
        first = false;
      } else {
        os << "|";
      }
      os << name.second;
    }
    os << "]";
    return os;
  }

 private:
  field_type v{};
};

}  // namespace sled
