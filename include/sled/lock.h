/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <condition_variable>
#include "sled/platform.h"

namespace sled::sync {

template <typename mtx_type>
class unlock_guard {
 public:
  explicit unlock_guard(mtx_type& m) : m_mtx(m) { m_mtx.unlock(); }
  ~unlock_guard() { m_mtx.lock(); }

 private:
  mtx_type& m_mtx;
};

template <typename mtx_type>
class lock_guard {
 public:
  explicit lock_guard(mtx_type& m) : m_mtx(m), m_locked(false) {
    m_mtx.lock();
    m_locked = true;
  }

  ~lock_guard() {
    if (m_locked) {
      m_mtx.unlock();
    }
  }

  void unlock() {
    debug_assert(m_locked);
    m_mtx.unlock();
    m_locked = false;
  }
  void lock() {
    debug_assert(!m_locked);
    m_mtx.lock();
    m_locked = true;
  }

  void wait(std::condition_variable& cv) {
    std::unique_lock<mtx_type> lock(m_mtx, std::adopt_lock);
    cv.wait(lock);
    lock.release();
  }

 private:
  mtx_type& m_mtx;
  bool m_locked;
};

using lock_mtx = lock_guard<std::mutex>;
using unlock_mtx = unlock_guard<std::mutex>;
};  // namespace sled::sync
