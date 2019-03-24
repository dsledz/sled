/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <filesystem>
#include "sled/platform.h"

namespace sled::fs {

/**
 * Domain specific wrapped file descriptor
 */
struct fd {
  fd() = default;
  ~fd();
  explicit fd(int new_fd) noexcept : fd_(new_fd) {}
  fd(const fd &rhs) = delete;
  fd(fd &&rhs) noexcept {
    fd_ = rhs.fd_;
    rhs.fd_ = -1;
  }
  fd &operator=(fd &&rhs) {
    if (this != &rhs) {
      fd_ = rhs.fd_;
      rhs.fd_ = -1;
    }
    return *this;
  }
  fd &operator=(int &&rhs) {
    if (fd_ != -1) {
      close();
    }
    fd_ = rhs;
    return *this;
  }

  void close();

  bool valid() { return fd_ != -1; }

  int value() { return fd_; }

  int fd_{-1};
};

/**
 * Represents an open file.
 *
 * Designed to work work sled::executor
 */
class open_file {
 public:
  open_file(const std::filesystem::path &path);
  ~open_file() = default;

  bool valid() { return fd_.valid(); }

  template <size_t SIZE>
  int read_into(std::array<uint8_t, SIZE> &dest) {
    return read_into(dest.data(), dest.size());
  }

  template <size_t SIZE>
  int read_into(std::array<std::byte, SIZE> &dest) {
    return read_into(dest.data(), dest.size());
  }

  int read_into(std::byte *data, int size);

  int read_into(uint8_t *data, int size) {
    return read_into(reinterpret_cast<std::byte *>(data), size);
  }

  template <size_t SIZE>
  int pread_into(std::array<uint8_t, SIZE> &dest, int offset) {
    return pread_into(reinterpret_cast<std::byte *>(dest.data()), dest.size(),
                      offset);
  }

  template <size_t SIZE>
  int pread_into(std::array<std::byte, SIZE> &dest, int offset) {
    return pread_into(dest.data(), dest.size(), offset);
  }

  template <typename T>
  int pread_into(std::vector<T> &dest, int offset) {
    return pread_into(reinterpret_cast<std::byte *>(dest.data()),
                      dest.size() * sizeof(T), offset);
  }

  template <typename T>
  int pread_into(T &t, int offset) {
    return pread_into(reinterpret_cast<std::byte *>(&t), sizeof(t), offset);
  }

  int pread_into(std::byte *data, int size, int offset);

 private:
  fd fd_{};
};

}  // namespace sled::fs
