/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/enum.h"
#include "sled/platform.h"

#include <atomic>

namespace sled {

template <typename T>
class atomic_flags {
 public:
  using field_type = typename std::underlying_type<T>::type;
  using nonatomic_type = sled::flags<T>;
  using cond_return_type = std::pair<bool, nonatomic_type>;

  atomic_flags() = default;
  atomic_flags(std::initializer_list<T> l) {
    field_type value{};
    for (auto b : l) {
      value |= static_cast<field_type>(b);
    }
    value_ = value;
  }
  explicit atomic_flags(field_type value) : value_(value) {}

  bool empty() const { return value_ == 0; }

  void zero() { value_ = 0; }

  field_type get() const { return value_; }

  operator nonatomic_type() const { return nonatomic_type{value_}; }

  a_forceinline bool is_set(T t) const {
    return (value_ & static_cast<field_type>(t)) != 0;
  }

  a_forceinline bool is_clear(T t) const {
    return (value_ & static_cast<field_type>(t)) == 0;
  }

  void clear(T t) { value_ &= ~static_cast<field_type>(t); }

  void set(T t) { value_ |= static_cast<field_type>(t); }

  cond_return_type set_cond(
      nonatomic_type const& to_set,
      nonatomic_type const& predicate_clear = nonatomic_type{},
      nonatomic_type const& predicate_set = nonatomic_type{}) {
    field_type set = to_set.get();
    field_type pred_clear = predicate_clear.get();
    field_type pred_set = predicate_set.get();
    field_type old = value_;
    field_type new_value;
    for (;;) {
      if ((old & pred_clear) != 0) {
        return std::make_pair(false, nonatomic_type(old));
      }
      if ((old & pred_set) != pred_set) {
        return std::make_pair(false, nonatomic_type(old));
      }
      new_value = old | set;

      if (std::atomic_compare_exchange_strong(&value_, &old, new_value)) {
        break;
      }
    }
    return std::make_pair(true, nonatomic_type(new_value));
  }

  cond_return_type clear_cond(nonatomic_type const& to_clear) {
    return update({}, to_clear);
  }

  nonatomic_type update(nonatomic_type const& to_set,
                        nonatomic_type const& to_clear) {
    field_type clear = to_clear.get();
    field_type set = to_set.get();
    field_type old = value_;
    field_type new_value;
    for (;;) {
      new_value = (old & ~clear) | set;

      if (std::atomic_compare_exchange_strong(&value_, &old, new_value)) {
        break;
      }
    }
    return nonatomic_type(new_value);
  }

  cond_return_type update_cond(nonatomic_type const& to_set,
                               nonatomic_type const& to_clear) {
    field_type clear = to_clear.get();
    field_type set = to_set.get();
    field_type old = value_;
    field_type new_value;
    for (;;) {
      if ((old & clear) != clear) {
        return std::make_pair(false, nonatomic_type(old));
      }
      if ((old & set) != 0) {
        return std::make_pair(false, nonatomic_type(old));
      }
      new_value = (old & ~clear) | set;

      if (std::atomic_compare_exchange_strong(&value_, &old, new_value)) {
        break;
      }
    }
    return std::make_pair(true, nonatomic_type(new_value));
  }

  bool compare_exchange(nonatomic_type const& old_value,
                        nonatomic_type const& new_value) {
    field_type old_v = old_value.get();
    field_type new_v = new_value.get();
    return std::atomic_compare_exchange_strong(&value_, &old_v, new_v);
  }

  atomic_flags<T>& operator|=(const atomic_flags<T>& rhs) {
    value_ |= rhs.value_;
    return *this;
  }

  atomic_flags<T>& operator|=(const T& rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  atomic_flags<T>& operator&=(const atomic_flags<T>& rhs) {
    value_ &= rhs.value_;
    return *this;
  }

  atomic_flags<T>& operator&=(const T& rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  atomic_flags<T> operator~() { return ~value_.load(); }

  friend nonatomic_type operator|(atomic_flags const& lhs,
                                  atomic_flags<T> const& rhs) {
    return nonatomic_type{lhs.value_.load() | rhs.value_.load()};
  }

  friend nonatomic_type operator|(atomic_flags const& lhs,
                                  nonatomic_type const& rhs) {
    return nonatomic_type{lhs.value_.load() | rhs.get()};
  }

  friend nonatomic_type operator|(nonatomic_type const& lhs,
                                  atomic_flags<T> const& rhs) {
    return nonatomic_type{lhs.get() | rhs.value_.load()};
  }

  friend bool operator==(atomic_flags const& lhs, atomic_flags const& rhs) {
    return lhs.value_ == rhs.value_;
  }

 private:
  std::atomic<field_type> value_{};
};

}  // namespace sled
