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

template <typename executor_t>
class FutureBase {
 private:
  enum FLAGS : uint64_t {
    EMPTY = 0x00,
    PENDING = 0x01,
    COMPLETED = 0x02,
    FINISHED = 0x04,
    HAZARD = 0x08,
  };

 protected:
  explicit FutureBase(bool completed = false) {
    if (completed) {
      flags_ = FLAGS{COMPLETED};
    }
  }

  bool get_helper() {
    FLAGS old_flags{COMPLETED};
    FLAGS flags{COMPLETED | FINISHED};

    bool b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);

    debug_assert((old_flags & (PENDING | FINISHED)) == 0);
    return b;
  }

  bool valid_helper() { return (flags_ & COMPLETED) != 0; }

  void wait_helper() {
  retry:
    FLAGS flags{FINISHED | COMPLETED};
    FLAGS old_flags{COMPLETED};
    if (std::atomic_compare_exchange_strong(&flags_, &old_flags, flags)) {
      return;
    }
    // Slow path.
    pending_ = executor_t::cur_task();
    // The first time, we try the simple case.
    debug_assert((old_flags & (FINISHED | COMPLETED)) == 0);
    // The future isn't ready yet.  Indicate our intent to wait.
    flags = FLAGS{PENDING};
    while (!std::atomic_compare_exchange_strong(&flags_, &old_flags, flags)) {
      // Handle race where set_result was called.
      if (old_flags & COMPLETED) {
        goto retry;
      }
    }
    bool b;
    do {
      // The executor will deal with the race between set() calling wake and
      // this task yielding.
      executor_t::cur_task_suspend();
      old_flags = FLAGS{flags_ | COMPLETED};
      flags = FLAGS{FINISHED | COMPLETED};
      if ((old_flags & HAZARD) != 0) {
        flags = FLAGS{flags | HAZARD};
      }

      // Check if future was completed.
      b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    } while (!b);

    do {
      // Wait until the hazard flag is cleared.
      b = (flags_ & HAZARD) == 0;
    } while (!b);

    // At this point, the set side should be done with the future.
  }

  void set_helper() {
    // Make sure the value_ wasn't already set
    debug_assert((flags_ & COMPLETED) == 0);
    bool b;
    FLAGS old_flags;
    FLAGS flags;
    do {
      old_flags = flags_;
      flags = FLAGS{COMPLETED | old_flags};
      if ((old_flags & PENDING) != 0) {
        // There is a pending waiter, mark a hazard.
        flags = FLAGS{flags | HAZARD};
      }
      debug_assert((old_flags & COMPLETED) == 0);
      b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
    } while (!b);

    if ((old_flags & PENDING) != 0) {
      // Wake up the pending task.
      // The pending task is protected by the HAZARD flag
      debug_assert(pending_ != nullptr);
      pending_->wake();

      do {
        // Clear the HAZARD flag
        old_flags = flags_;
        flags = FLAGS{COMPLETED | FINISHED};

        b = std::atomic_compare_exchange_strong(&flags_, &old_flags, flags);
      } while (!b);
    }
    // future can deallocated at this point, so avoid touching things.
  }

  std::atomic<FLAGS> flags_{EMPTY};
  typename executor_t::task *pending_{nullptr};
};

/**
 * Future.
 *
 * A future is shared between exactly two tasks.  The first task is expected to
 * set the value of the future while the second thread must eventually call get
 */
template <typename result_t, typename executor_t>
class Future : public FutureBase<executor_t> {
 public:
  Future() = default;
  Future(result_t &&value) : FutureBase<executor_t>(true), value_(value) {}
  Future(Future const &rhs) = delete;
#if 0
  // XXX: Make Future moveable
  Future(Future &&rhs) = default;
  Future& operator=(Future &&rhs) = default;
#endif

  std::optional<result_t> get() {
    if (this->get_helper()) {
      return std::move(value_);
    } else {
      return std::nullopt;
    }
  }

  bool valid() { return this->valid_helper(); }

  result_t wait() {
    this->wait_helper();
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
    // Update our value
    if constexpr (std::is_move_constructible<result_t>::value) {
      value_ = std::forward<result_t>(value);
    } else {
      value_ = value;
    }
    // Make our value visible
    this->set_helper();
    // future can deallocated at this point, so avoid touching things.
  }

  /**
   * Set the result. non-rvalue version
   */
  void set_result(result_t const &value) {
    // XXX: copy of rvalue version.
    // Update our value
    value_ = value;
    // Make our value visible
    this->set_helper();
    // future can deallocated at this point, so avoid touching things.
  }

 private:
  std::optional<result_t> value_{std::nullopt};
};

/**
 * Future, void return.
 *
 * A future is shared between exactly two tasks.  The first task is expected to
 * set the value of the future while the second thread must eventually call get
 */
template <typename executor_t>
class Future<void, executor_t> : FutureBase<executor_t> {
 public:
  Future() = default;
  Future(Future const &rhs) = delete;

  std::optional<bool> get() {
    if (this->get_helper()) {
      return true;
    } else {
      return std::nullopt;
    }
  }

  bool valid() { return this->valid_helper(); }

  void wait() {
    this->wait_helper();
    // nothing to do here because our value is void.
  }

  /**
   * Set the result. non-rvalue version
   */
  void set_result() {
    // XXX: copy of rvalue version.
    // Make our value (which is actually void) visible.
    this->set_helper();
    // future can deallocated at this point, so avoid touching things.
  }
};

}  // namespace sled::executor
