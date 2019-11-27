/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <cstdint>
#include <iomanip>
#include <memory>
#include <ostream>
#include <type_traits>

namespace sled {

/**
 * Formatter object.
 *
 * Objects supporting this interface should implement the following functions:
 * std::string fmt_read(T *x);
 * std::ostream &operator<<(std::ostream &os, T *x);
 */
class fmt {
 public:
  template <typename T>
  // NOLINTNEXTLINE
  fmt(T x) : self_(std::make_shared<model<T> >(std::move(x))) {}
  fmt(std::string x) : self_(std::make_shared<str_model>(std::move(x))) {}

  fmt(const fmt &x) : self_(x.self_->copy_()) {}
  fmt(fmt &&) noexcept = default;
  fmt &operator=(const fmt &x) { return *this = fmt(x); }
  fmt &operator=(fmt &&) noexcept = default;

  operator std::string() const { return self_->fmt_read_(); }

  friend std::string fmt_read(const fmt &x) { return x.self_->fmt_read_(); }

  friend std::ostream &operator<<(std::ostream &os, const fmt &x) {
    return x.self_->fmt_ostream_(os);
  }

 private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual std::unique_ptr<concept_t> copy_() const = 0;
    virtual std::string fmt_read_() const = 0;
    virtual std::ostream &fmt_ostream_(std::ostream &os) const = 0;
  };
  template <typename T>
  struct model final : concept_t {
    explicit model(T x) : data_(std::move(x)) {}
    std::unique_ptr<concept_t> copy_() const override {
      return std::make_unique<model>(*this);
    }
    std::string fmt_read_() const final { return fmt_read(data_); }
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
    std::string fmt_read_() const final { return data_; }
    std::ostream &fmt_ostream_(std::ostream &os) const final {
      os << data_;
      return os;
    }
    std::string data_;
  };
  std::shared_ptr<const concept_t> self_;
};

};  // namespace sled
