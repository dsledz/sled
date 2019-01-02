/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/channel.h"
#include "sled/platform.h"

namespace sled::sync {

/**
 * Sleepable mutual exclusion lock with signaling
 */
template <typename Executor>
class mutex {
 public:
  void lock() {
    auto lo = channel_.get();
    // Current task now owns the lock, update.
    owner_ = Executor::current_task_id();
  }

  void unlock() {
    // Release the lock, notifying any waiters.
    channel_.put(owner_);
  }

  /**
   * Retrieve the current owner.
   *
   * This function is only reliable in determining if the current task is the
   * owner.
   *
   * @return snapshot of the mutex owner.
   */
  TaskId owner() const { return owner_; }

 private:
  spinlock spinlock_{};
  TaskId owner_{};
  channel<TaskId, Executor> channel_{};
};

}  // namespace sled::sync
