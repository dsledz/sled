/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#pragma once

#include <deque>
#include <optional>

#include "sled/lock.h"
#include "sled/ring.h"
#include "sled/spinlock.h"

namespace sled::executor {

/**
 * Type-safe object channel.
 */
template <class object_t, class executor_t>
class Channel {
 public:
  using lock_t = sled::sync::SpinLock;
  using lock_guard_t = sled::sync::lock_guard<lock_t>;

  Channel() : m_objects() {}
  ~Channel() = default;
  Channel(const Channel &ch) = delete;

  /**
   * Place an object in the channel.
   */
  void put(object_t obj) {
    lock_guard_t lock(m_mtx);
    bool b = m_objects.push_back(obj);
    while (!b) {
      // Channel is full, sleep
      lock.unlock();
      executor_t::yield();
      lock.lock();
      b = m_objects.push_back(obj);
    }
    if (m_waiting) {
      m_waiting->wake();
    }
  }

  int size() {
    lock_guard_t lock(m_mtx);
    return m_objects.size();
  }

  /**
   * Remove an object from the channel, blocking if no object
   * exists.
   */
  object_t get() {
    lock_guard_t lock(m_mtx);
    assert(m_waiting == NULL);
    auto *task = executor_t::cur_task();
    while (m_objects.empty()) {
      m_waiting = task;
      lock.unlock();
      task->suspend();
      lock.lock();
    }
    object_t obj = m_objects.front();
    m_objects.pop_front();
    m_waiting = nullptr;
    return obj;
  }

  /**
   * Remove an object from the channel, returning an empty object
   * if none exist.
   */
  std::optional<object_t> try_get() {
    lock_guard_t lock(m_mtx);
    if (!m_objects.empty()) {
      object_t obj = m_objects.front();
      m_objects.pop_front();
      return obj;
    }
    return std::nullopt;
  }

 private:
  lock_t m_mtx;
  typename executor_t::task *m_waiting{};
  sled::ring<object_t, 16> m_objects;
};

/**
 * Type-safe object channel.
 */
template <class object_t>
class SyncChannel {
 public:
  using lock_t = std::mutex;
  using lock_cv_t = std::condition_variable;
  using lock_guard_t = sled::sync::lock_guard<lock_t>;

  SyncChannel() : m_objects() {}
  ~SyncChannel() = default;
  SyncChannel(const SyncChannel &ch) = delete;

  /**
   * Returns true if the channel is empty.
   */
  bool empty() const { return m_objects.empty(); }

  /**
   * Place an object in the channel.
   */
  void put(object_t obj) {
    lock_guard_t lock(m_mtx);
    m_objects.push_back(obj);
    m_cv.notify_all();
  }

  void close() {
    lock_guard_t lock(m_mtx);
    m_closed = true;
    m_cv.notify_all();
  }

  /**
   * Remove an object from the channel, blocking if no object exists.
   * Returns nullopt iff the channel is closed.
   */
  std::optional<object_t> get() {
    lock_guard_t lock(m_mtx);
    while (m_objects.empty()) {
      if (m_closed) {
        return std::nullopt;
      }
      lock.wait(m_cv);
    }
    auto obj{m_objects.front()};
    m_objects.pop_front();
    return obj;
  }

  /**
   * Remove an object from the channel, returning an empty object
   * if none exist.
   */
  std::optional<object_t> try_get() {
    lock_guard_t lock(m_mtx);
    if (!m_objects.empty()) {
      auto obj{m_objects.front()};
      m_objects.pop_front();
      return obj;
    }
    return std::nullopt;
  }

 private:
  lock_t m_mtx;
  lock_cv_t m_cv;
  std::deque<object_t> m_objects;
  bool m_closed{false};
};

}  // namespace sled::executor
