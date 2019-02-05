/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"
#include "sled/exception.h"

namespace sled::net {

/**
 * Wrapped integer representing a networking error.
 *
 */
struct net_error {
  net_error() = default;
  explicit net_error(int err) : errno_(err) {}

  void reset() { errno_ = 0; }
  bool error() { return errno_ != 0; }

  inline operator bool() {
    return error();
  }

  const char *what() const noexcept {
    // TODO(dan): Make this reentrant
    auto *str = strerror(errno_);
    return str;
  }

  friend std::ostream &operator<<(std::ostream &os, const net_error &rhs) {
    os << rhs.what();
    return os;
  }

  int errno_{0};
};

/**
 * Networking layer result.
 *
 * This is a domain specific implementation of a combination of std::optional
 * and expected.
 */
struct net_result {
  net_result() = default;
  explicit net_result(int result) noexcept;
  net_result(const net_result &rhs) = delete;
  net_result(net_result &&rhs) noexcept {
    result_ = rhs.result_;
    rhs.result_ = -1;
  }
  net_result& operator=(net_result &&rhs) noexcept {
    if (this != &rhs) {
      result_ = rhs.result_;
      error_ = rhs.error_;
      rhs.result_ = -1;
      rhs.error_.reset();
    }
    return *this;
  }
  net_result& operator=(int rhs) noexcept {
    *this = net_result(rhs);
    return *this;
  }

  void reset() {
    result_ = -1;
    error_.reset();
  }

  bool error() { return error_; }

  /**
   * Retrieve the result, throwing if there was an error.
   */
  int result();

  /**
   * Optionally retrieve the result, otherwise error.
   */
  std::optional<int> maybe_result() noexcept;

  int result_{-1};
  net_error error_{};
};

/**
 * Domain specific wrapped file descriptor
 */
struct net_fd {
  net_fd() = default;
  ~net_fd();
  explicit net_fd(int new_fd) noexcept : fd_(new_fd) {}
  net_fd(net_result result) : fd_(result.result()) {}
  net_fd(const net_fd &rhs) = delete;
  net_fd(net_fd &&rhs) noexcept {
    fd_ = rhs.fd_;
    rhs.fd_ = -1;
  }
  net_fd& operator=(net_fd &&rhs) {
    if (this != &rhs) {
      fd_ = rhs.fd_;
      rhs.fd_ = -1;
    }
    return *this;
  }

  bool valid() { return fd_ != -1; }

  int value() { return fd_; }

  int fd_{-1};
};

struct net_exception : public sled::Exception {
  explicit net_exception(net_error err)
      : Exception("Network exception: ", err), err_(err) {}

  net_error err_;
};

/**
 * IPv4 address.
 */
struct ip_address {
  constexpr ip_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d) :
      v(((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | ((uint8_t)d << 0)) {}

  uint32_t nbo() noexcept;

  uint32_t v; /**< IP address in host order */
};

/**
 * IP port
 */
struct ip_port {
  constexpr ip_port(unsigned short v) : v(v) {}

  uint16_t nbo() noexcept;

  uint16_t v; /**< Port address in host order */
};

} // namespace sled::net
