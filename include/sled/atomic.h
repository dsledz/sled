/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/enum.h"
#include "sled/platform.h"

namespace sled {

template <typename T>
class AtomicBitField {
 public:
  using field_type = typename std::underlying_type<T>::type;
  using nonatomic_type = sled::BitField<T>;

  AtomicBitField() = default;
  AtomicBitField(std::initializer_list<T> l) {
    field_type value{};
    for (auto b : l) {
      value |= static_cast<field_type>(b);
    }
    value_ = value;
  }
  explicit AtomicBitField(field_type value) : value_(value) {}

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

  bool compare_exchange(nonatomic_type const& old_value,
                        nonatomic_type const& new_value) {
    field_type old_v = old_value.get();
    field_type new_v = new_value.get();
    return std::atomic_compare_exchange_strong(&value_, &old_v, new_v);
  }

  AtomicBitField<T>& operator|=(const AtomicBitField<T>& rhs) {
    value_ |= rhs.value_;
    return *this;
  }

  AtomicBitField<T>& operator|=(const T& rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  AtomicBitField<T>& operator&=(const AtomicBitField<T>& rhs) {
    value_ &= rhs.value_;
    return *this;
  }

  AtomicBitField<T>& operator&=(const T& rhs) {
    value_ |= static_cast<field_type>(rhs);
    return *this;
  }

  AtomicBitField<T> operator~() { return ~value_.load(); }

  friend nonatomic_type operator|(AtomicBitField const& lhs,
                                  AtomicBitField<T> const& rhs) {
    return nonatomic_type{lhs.value_.load() | rhs.value_.load()};
  }

  friend nonatomic_type operator|(AtomicBitField const& lhs,
                                  nonatomic_type const& rhs) {
    return nonatomic_type{lhs.value_.load() | rhs.get()};
  }

  friend nonatomic_type operator|(nonatomic_type const& lhs,
                                  AtomicBitField<T> const& rhs) {
    return nonatomic_type{lhs.get() | rhs.value_.load()};
  }

  friend bool operator==(AtomicBitField const& lhs, AtomicBitField const& rhs) {
    return lhs.value_ == rhs.value_;
  }

 private:
  std::atomic<field_type> value_{};
};

}  // namespace sled
