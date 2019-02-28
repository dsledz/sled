/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace sled {

/*  _____                    _   _
 * | ____|_  _____ ___ _ __ | |_(_) ___  _ __
 * |  _| \ \/ / __/ _ \ '_ \| __| |/ _ \| '_ \
 * | |___ >  < (_|  __/ |_) | |_| | (_) | | | |
 * |_____/_/\_\___\___| .__/ \__|_|\___/|_| |_|
 *                    |_|
 */

/**
 * General exception
 */
struct Exception : public std::exception {
  const char *what() const noexcept override { return msg_.c_str(); }

  explicit Exception(std::string msg) : msg_(std::move(msg)) {}

  template <typename... Args>
  explicit Exception(Args... args) {
    std::stringstream ss;
    build(ss, args...);
    msg_ = ss.str();
  }

 protected:
  template <typename T>
  void build(std::stringstream &ss, T &&arg) {
    ss << std::forward<T>(arg);
  }

  template <typename T, typename... Args>
  void build(std::stringstream &ss, T &&arg, Args... args) {
    ss << std::forward<T>(arg);
    build(ss, args...);
  }

  std::string msg_;
};

/**
 * Conversion Error
 */
struct ConversionError : public Exception {
 public:
  explicit ConversionError(const std::string &type)
      : Exception("ConversionError: ", type), value() {}
  explicit ConversionError(const std::string &type, const std::string &v)
      : Exception("ConversionError: ", type, ", \"", v, "\""), value(v) {}
  // TODO(dan): operator<< constructor version.

  std::string value;
};

}  // namespace sled
