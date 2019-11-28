/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"

#include <cstdint>
#include <iomanip>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>

namespace sled {

/**
 * Formatter object.  Represents a value to be formatted.
 *
 * Objects supporting this concept should implement the following functions:
 * std::string fmt_string(const T &x);
 * std::ostream &operator<<(std::ostream &os, const T &x);
 */
class fmt_obj {
 public:
  template <typename T>
  // NOLINTNEXTLINE
  fmt_obj(T x) : self_(std::make_shared<model<T> >(std::move(x))) {}
  fmt_obj(std::string x) : self_(std::make_shared<str_model>(std::move(x))) {}
  fmt_obj(const char *x) : self_(std::make_shared<str_model>(std::string(x))) {}

  fmt_obj(const fmt_obj &x) : self_(x.self_->copy_()) {}
  fmt_obj(fmt_obj &&) noexcept = default;
  fmt_obj &operator=(const fmt_obj &x) { return *this = fmt_obj(x); }
  fmt_obj &operator=(fmt_obj &&) noexcept = default;

  operator std::string() const { return self_->fmt_string_(); }

  friend std::string fmt_string(const fmt_obj &x) { return x.self_->fmt_string_(); }

  friend std::ostream &operator<<(std::ostream &os, const fmt_obj &x) {
    return x.self_->fmt_ostream_(os);
  }

 private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual std::unique_ptr<concept_t> copy_() const = 0;
    virtual std::string fmt_string_() const = 0;
    virtual std::ostream &fmt_ostream_(std::ostream &os) const = 0;
  };
  template <typename T>
  struct model final : concept_t {
    explicit model(T x) : data_(std::move(x)) {}
    std::unique_ptr<concept_t> copy_() const override {
      return std::make_unique<model>(*this);
    }
    std::string fmt_string_() const final { return fmt_string(data_); }
    std::ostream &fmt_ostream_(std::ostream &os) const final {
      os << data_;
      return os;
    }

    T data_;
  };
  struct str_model final : concept_t {
    explicit str_model(std::string x) : data_(std::move(x)) {}
    std::unique_ptr<concept_t> copy_() const override {
      return std::make_unique<str_model>(*this);
    }
    std::string fmt_string_() const final { return data_; }
    std::ostream &fmt_ostream_(std::ostream &os) const final {
      os << data_;
      return os;
    }
    std::string data_;
  };
  std::shared_ptr<const concept_t> self_;
};

/**
 * Recursively convert the formatters to a string.
 */
static a_forceinline std::string format(const sled::fmt_obj &p) {
  return fmt_string(p);
}
template <typename... T>
std::string format(const sled::fmt_obj &p, T const &... t) {
  return fmt_string(p) + sled::format(t...);
}

/**
 * Recursively convert the formatters to a string.
 */
static a_forceinline void format_os(std::ostream & /*os*/) {}
template <typename... T>
void format_os(std::ostream &os, const sled::fmt_obj &p, T const &... t) {
  os << p;
  sled::format_os(os, t...);
}

};  // namespace sled
