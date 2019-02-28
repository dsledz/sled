/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"

#include <cstddef>

namespace sled {

/**
 * Arbitary fixed bit number
 */
template <int WIDTH>
struct fixed_bits {
  fixed_bits() = default;
  explicit constexpr fixed_bits(uint64_t v) : v(v) {}

  constexpr int width() const { return WIDTH; }

  uint64_t v{0};
};

/**
 * Variable number of bits number.
 */
struct variable_bits {
  variable_bits() = default;
  explicit constexpr variable_bits(uint64_t v, int num_bits)
      : v(v), w(num_bits) {}

  constexpr int width() const { return w; }

  uint64_t v{0};
  int w{0};
};

/**
 * Byte stream interface
 */
class bytestream {
 public:
  bytestream(std::byte *begin, std::byte *end) : begin_(begin), end_(end) {}
  ~bytestream() = default;

  int size() { return static_cast<int>(end_ - begin_); }

  void write(int offset, std::byte value) {
    assert(end_ - begin_ > offset);
    memcpy(begin_ + offset, &value, sizeof(value));
  }

  std::byte read(int offset) { return begin_[offset]; }

  void pwrite(std::byte *value, int length, int offset) {
    memcpy(begin_ + offset, value, length);
  }

  void pread(std::byte *value, int length, int offset) {
    memcpy(value, begin_ + offset, length);
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void pread(T &value, int offset) {
    memcpy(reinterpret_cast<std::byte *>(&value), begin_ + offset,
           sizeof(value));
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void le_pwrite(T v, int offset) {
    if constexpr (sizeof(T) == 2) {
      uint16_t t;
      mempcy(&t, &v, sizeof(t));
      t = htole16(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else if constexpr (sizeof(T) == 4) {
      uint32_t t;
      mempcy(&t, &v, sizeof(t));
      t = htole32(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else if constexpr (sizeof(T) == 8) {
      uint64_t t;
      mempcy(&t, &v, sizeof(t));
      t = htole64(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else {
      memcpy(begin_ + offset, &v, sizeof(v));
    }
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void be_pwrite(T v, int offset) {
    if constexpr (sizeof(T) == 2) {
      uint16_t t;
      mempcy(&t, &v, sizeof(t));
      t = htobe16(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else if constexpr (sizeof(T) == 4) {
      uint32_t t;
      mempcy(&t, &v, sizeof(t));
      t = htobe32(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else if constexpr (sizeof(T) == 8) {
      uint64_t t;
      mempcy(&t, &v, sizeof(t));
      t = htobe64(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else {
      memcpy(begin_ + offset, &v, sizeof(v));
    }
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  T be_pread(int offset) {
    T r;
    if constexpr (sizeof(T) == 2) {
      uint16_t t;
      memcpy(&t, begin_ + offset, sizeof(t));
      t = be16toh(t);
      memcpy(&r, &t, sizeof(r));
    } else if constexpr (sizeof(T) == 4) {
      uint32_t t;
      memcpy(&t, begin_ + offset, sizeof(t));
      t = be32toh(t);
      memcpy(&r, &t, sizeof(r));
    } else if constexpr (sizeof(T) == 8) {
      uint64_t t;
      memcpy(&t, begin_ + offset, sizeof(t));
      t = be64toh(t);
      memcpy(&r, &t, sizeof(r));
    } else {
      memcpy(&r, begin_ + offset, sizeof(r));
    }
    return r;
  }

  uint64_t be_pread(int num_bytes, int offset) {
    assert(num_bytes > 0 && num_bytes <= 8);
    uint64_t r = 0;
    std::byte *buf = reinterpret_cast<std::byte *>(&r);
    // Copy the bytes into the correct BE position
    for (int i = num_bytes; i > 0; i--, offset++) {
      memcpy(&buf[8 - i], &begin_[offset], sizeof(std::byte));
    }
    return be64toh(r);
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  T le_pread(int offset) {
    T r;
    if constexpr (sizeof(T) == 2) {
      uint16_t t;
      memcpy(&t, begin_ + offset, sizeof(t));
      t = le16toh(t);
      memcpy(&r, &t, sizeof(r));
    } else if constexpr (sizeof(T) == 4) {
      uint32_t t;
      memcpy(&t, begin_ + offset, sizeof(t));
      t = le32toh(t);
      memcpy(&r, &t, sizeof(r));
    } else if constexpr (sizeof(T) == 8) {
      uint64_t t;
      memcpy(&t, begin_ + offset, sizeof(t));
      t = le64toh(t);
      memcpy(&r, &t, sizeof(r));
    } else {
      memcpy(&r, begin_ + offset, sizeof(r));
    }
    return r;
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void be_write(T v, int offset) {
    if constexpr (sizeof(T) == 2) {
      uint16_t t;
      mempcy(&t, &v, sizeof(t));
      t = htobe16(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else if constexpr (sizeof(T) == 4) {
      uint32_t t;
      mempcy(&t, &v, sizeof(t));
      t = htobe32(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else if constexpr (sizeof(T) == 8) {
      uint64_t t;
      mempcy(&t, &v, sizeof(t));
      t = htobe64(t);
      memcpy(begin_ + offset, &t, sizeof(t));
    } else {
      memcpy(begin_ + offset, &v, sizeof(v));
    }
  }

  /**
   * Return a fixed number of bits at the specifid bit offset.
   *
   * Bits are stored in big endian format.
   * be_pread<fixed_bits<1>>(0) would read the highest bit of the first byte
   * of the bytestream.
   */
  template <int WIDTH>
  fixed_bits<WIDTH> be_bits_pread(int offset) {
    fixed_bits<WIDTH> fb;
    be_bits_pread_into(fb, WIDTH, offset);
    return fb;
  }

  /**
   * Return variable number of bits at the specified offset.
   */
  variable_bits be_bits_pread(int num_bits, int offset) {
    variable_bits vb;
    vb.w = num_bits;
    be_bits_pread_into(vb, num_bits, offset);
    return vb;
  }

 private:
  template <typename T>
  void be_bits_pread_into(T &bits, int num_bits, int offset) {
    int byte_offset = offset / 8;
    int bits_offset = offset % 8;
    int bits_left = num_bits;
    while (bits_left > 0) {
      /* Extract the current bits from the byte */
      int high = 8 - bits_offset;
      int low = std::max(high - bits_left, 1);
      std::byte b = begin_[byte_offset];
      // Mask the high bits
      b = b & std::byte((1u << high) - 1);
      // Shift away the low bits
      b = b >> (low - 1);
      bits_left -= high - low + 1;
      bits.v |= std::to_integer<uint64_t>(b) << bits_left;
      bits_offset = 0;
      byte_offset++;
    }
  }

  std::byte *begin_{nullptr};
  std::byte *end_{nullptr};
};

/**
 * Fixed length bytestream interface
 */
template <size_t SIZE>
class fixed_bytestream {
 public:
  fixed_bytestream() : bs_(data_.begin(), data_.end()) {}
  ~fixed_bytestream() = default;

  bytestream &bs() { return bs_; }

  int size() { return bs_.size(); }

  void write(int offset, std::byte value) { bs_.write(offset, value); }

  template <int offset>
  void write(std::byte value) {
    static_assert(offset + sizeof(std::byte) <= SIZE);
    bs_.write(offset, value);
  }

  void pwrite(std::byte *value, int length, int offset) {
    bs_.pwrite(value, length, offset);
  }

  template <int offset>
  void pwrite(std::byte *value, int length) {
    static_assert(offset + length <= SIZE);
    bs_.pwrite(value, length, offset);
  }

  std::byte read(int offset) { return bs_.read(offset); }

  template <int offset>
  std::byte read() {
    static_assert(offset + sizeof(std::byte) <= SIZE);
    return bs_.read(offset);
  }

  void pread(std::byte *value, int length, int offset) {
    bs_.pread(value, length, offset);
  }

  template <int offset>
  void pread(std::byte *value, int length) {
    bs_.pread(value, length, offset);
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void le_pwrite(T value, int offset) {
    bs_.le_pwrite<T>(value, offset);
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void be_pwrite(T value, int offset) {
    bs_.be_pwrite<T>(value, offset);
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  T be_pread(int offset) {
    return bs_.be_pread<T>(offset);
  }

  uint64_t be_pread(int num_bytes, int offset) {
    return bs_.be_pread(num_bytes, offset);
  }

  template <typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  T le_pread(int offset) {
    return bs_.le_pread<T>(offset);
  }

  template <int offset, typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void le_pwrite(T value) {
    static_assert(offset + sizeof(T) <= SIZE);
    bs_.le_pwrite<T>(value, offset);
  }

  template <int offset, typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  void be_pwrite(T value) {
    static_assert(offset + sizeof(T) <= SIZE);
    bs_.be_pwrite<offset, T>(value, offset);
  }

  template <int offset, typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  T be_pread() {
    static_assert(offset + sizeof(T) <= SIZE);
    return bs_.be_pread<T>(offset);
  }

  template <int offset, typename T,
            class = std::enable_if_t<std::is_trivially_copyable_v<T>>>
  T le_pread() {
    static_assert(offset + sizeof(T) <= SIZE);
    return bs_.le_pread<T>(offset);
  }

  std::array<std::byte, SIZE> &buf() { return data_; };

 private:
  std::array<std::byte, SIZE> data_{};
  bytestream bs_;
};

}  // namespace sled
