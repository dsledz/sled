/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <atomic>
#include <optional>

#include "sled/platform.h"
#include "sled/type_traits.h"

namespace sled::executor {

/**
 * Future.
 *
 * A future is shared between exactly two tasks.  The first task is expected to
 * set the value of the future while the second thread must eventually call get
 */
template <typename result_t, typename executor_t>
class Future {
 private:
  enum FLAGS : uint64_t {
    EMPTY = 0x00,
    PENDING = 0x01,
    COMPLETED = 0x02,
    FINISHED = 0x04,
    HAZARD = 0x08,
  };

 public:
  Future() = default;
  Future(result_t &&value) : value_(value), flags_(COMPLETED) {}
  Future(Future const &rhs) = delete;
#if 0
  // XXX: Make Future moveable
  Future(Future &&rhs) = default;
  Future& operator=(Future &&rhs) = default;
#endif

  std::optional<result_t> get() {
    FLAGS old_flags{COMPLETED};
    FLAGS flags{COMPLETED | FINISHED};

    bool b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    if (b) {
      // Future is completed, return
      return std::move(value_);
    }

    debug_assert((old_flags & (PENDING | FINISHED)) == 0);
    return std::nullopt;
  }

  bool valid() { return (flags_ & COMPLETED) != 0; }

  result_t wait() {
    FLAGS flags{FINISHED | COMPLETED};
    FLAGS old_flags{COMPLETED};
    auto task = executor_t::cur_task();
    pending_ = task;
    for (;;) {
      // The first time, we try the simple case.
      if (std::atomic_compare_exchange_strong(&flags_, &old_flags, flags)) {
        break;
      }
      debug_assert((old_flags & (FINISHED | COMPLETED)) == 0);
      // The future isn't ready yet.  Indicate our intent to wait.
      flags = FLAGS{PENDING | HAZARD};
      while (std::atomic_compare_exchange_strong(&flags_, &old_flags, flags)) {
        // Repeat until we were able to trigger our intent
      }
      // The executor will deal with the race between set() calling wake and
      // this task yielding.
      executor_t::cur_task_suspend();
      old_flags = FLAGS{flags | COMPLETED};
      flags = FLAGS{FINISHED | COMPLETED};
    }
    if constexpr (std::is_move_constructible<result_t>::value) {
      return std::move(value_.value());
    } else {
      return value_.value();
    }
  }

  /**
   * Set the result.
   */
  void set_result(result_t &&value) {
    // Make sure the value_ wasn't already set
    debug_assert((flags_ & COMPLETED) == 0);
    // Update our value
    if constexpr (std::is_move_constructible<result_t>::value) {
      value_ = std::forward<result_t>(value);
    } else {
      value_ = value;
    }
    // Make our value visible
    bool b;
    FLAGS old_flags;
    do {
      old_flags = flags_;
      FLAGS flags{COMPLETED | old_flags};
      debug_assert((old_flags & COMPLETED) == 0);
      b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    } while (!b);

    if ((old_flags & PENDING) != 0) {
      // Wake up the pending task.
      debug_assert(pending_ != nullptr);
      pending_->wake();
    }
    // future can deallocated at this point, so avoid touching things.
  }

  /**
   * Set the result. non-rvalue version
   */
  void set_result(result_t const &value) {
    // XXX: copy of rvalue version.
    // Make sure the value_ wasn't already set
    debug_assert((flags_ & COMPLETED) == 0);
    // Update our value
    value_ = value;
    // Make our value visible
    bool b;
    FLAGS old_flags;
    do {
      old_flags = flags_;
      FLAGS flags{COMPLETED | old_flags};
      debug_assert((old_flags & COMPLETED) == 0);
      b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    } while (!b);

    if ((old_flags & PENDING) != 0) {
      // Wake up the pending task.
      debug_assert(pending_ != nullptr);
      pending_->wake();
    }
    // future can deallocated at this point, so avoid touching things.
  }

 private:
  std::optional<result_t> value_{std::nullopt};
  std::atomic<FLAGS> flags_{EMPTY};
  typename executor_t::task *pending_{nullptr};
};

/**
 * Future, void return.
 *
 * A future is shared between exactly two tasks.  The first task is expected to
 * set the value of the future while the second thread must eventually call get
 */
template <typename executor_t>
class Future<void, executor_t> {
 private:
  enum FLAGS : uint64_t {
    EMPTY = 0x00,
    PENDING = 0x01,
    COMPLETED = 0x02,
    FINISHED = 0x04,
    HAZARD = 0x08,
  };

 public:
  Future() = default;
  Future(Future const &rhs) = delete;

  std::optional<bool> get() {
    FLAGS old_flags{COMPLETED};
    FLAGS flags{COMPLETED | FINISHED};

    bool b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    if (b) {
      return true;
    }

    debug_assert((old_flags & (PENDING | FINISHED)) == 0);
    return std::nullopt;
  }

  bool valid() { return (flags_ & COMPLETED) != 0; }

  void wait() {
    FLAGS flags{FINISHED | COMPLETED};
    FLAGS old_flags{COMPLETED};
    auto task = executor_t::cur_task();
    pending_ = task;
    for (;;) {
      // The first time, we try the simple case.
      if (std::atomic_compare_exchange_strong(&flags_, &old_flags, flags)) {
        break;
      }
      debug_assert((old_flags & (FINISHED | COMPLETED)) == 0);
      // The future isn't ready yet.  Indicate our intent to wait.
      flags = FLAGS{PENDING | HAZARD};
      while (std::atomic_compare_exchange_strong(&flags_, &old_flags, flags)) {
        // Repeat until we were able to trigger our intent
      }
      // The executor will deal with the race between set() calling wake and
      // this task yielding.
      executor_t::cur_task_suspend();
      old_flags = FLAGS{flags | COMPLETED};
      flags = FLAGS{FINISHED | COMPLETED};
    }
    return;
  }

  /**
   * Set the result. non-rvalue version
   */
  void set_result() {
    // XXX: copy of rvalue version.
    // Make sure the value_ wasn't already set
    debug_assert((flags_ & COMPLETED) == 0);
    // Make our value visible
    bool b;
    FLAGS old_flags;
    do {
      old_flags = flags_;
      FLAGS flags{COMPLETED | old_flags};
      debug_assert((old_flags & COMPLETED) == 0);
      b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    } while (!b);

    if ((old_flags & PENDING) != 0) {
      // Wake up the pending task.
      debug_assert(pending_ != nullptr);
      pending_->wake();
    }
    // future can deallocated at this point, so avoid touching things.
  }

 private:
  std::atomic<FLAGS> flags_{EMPTY};
  typename executor_t::task *pending_{nullptr};
};

}  // namespace sled::executor
