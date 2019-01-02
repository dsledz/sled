/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <atomic>
#include "sled/platform.h"

namespace sled::sync {

class SpinLock {
 public:
  SpinLock() = default;

  inline void lock() {
    // TODO(dan): thread for debugging
    bool b;
    do {
      uint64_t unlocked = UNLOCKED;
      b = std::atomic_compare_exchange_strong(&m_lock, &unlocked, LOCKED);
    } while (!b);
  }

  inline void unlock() {
    uint64_t locked = LOCKED;
    std::atomic_compare_exchange_strong(&m_lock, &locked, UNLOCKED);
  }

 private:
  static constexpr uint64_t UNLOCKED = 0;
  static constexpr uint64_t LOCKED = 1;
  std::atomic<uint64_t> m_lock{UNLOCKED};
};

}  // namespace sled::sync
