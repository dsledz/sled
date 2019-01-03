/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/task.h"

namespace sled::executor {

/**
 * Executor Concept.
 *
 * Defines the interface/concept for an executor.
 */
class Executor {
 public:
  Executor() = default;
  virtual ~Executor() = default;
  Executor(Executor const &) = delete;

  using task = sled::executor::Task;

#if 0
  /**
   * Executor strand context.
   *
   * All work within a context occurs on a strand.
   */
  class Strand {
   private:
    explicit Strand(Executor *exec_ctx): exec_ctx_(exec_ctx) { }
   public:
    ~Strand() {
      if (exec_ctx_) {
        exec_ctx_->unadopt_thread(task_);
      }
    }
    Strand(Strand const&) = delete;
    Strand(Strand &&rhs) {
      std::swap(exec_ctx_, rhs.exec_ctx_);
      std::swap(task_, rhs.task_);
    }
    // XXX: move operator
   private:
    Executor *exec_ctx_{nullptr};
    Task *task_{nullptr};
  };

  /**
   * Convert a thread into an executor context.
   */
  virtual Strand create_thread();
#endif

  /**
   * Adopt thread.
   *
   * Marks the current thread usable by this executor.  The current thread must
   * not be associated with an executor.
   *
   * @return Task representing the current execution.
   */
  virtual Task *adopt_thread() = 0;

  /**
   * Unadopt thread.
   *
   * Marks the current thread as not usable by this executor.  This current
   * thread must have already be adopted by the executor.  The thread must not
   * be within the executor.
   */
  virtual void unadopt_thread(Task *task) = 0;

  /**
   * Resume.
   *
   * Resume execution of the exeutor on the current thread.
   */
  virtual void resume() = 0;

  /**
   * Resume execution to process any pending tasks.
   *
   * Resume execution of the executor on the current thread for one task.
   */
  virtual void resume_pending() = 0;

  /**
   * Run a task.
   *
   * Queues the current task for execution.
   */
  virtual void run() = 0;

  /**
   * Shutdown.
   *
   * Shutdown the executor.
   */
  virtual void shutdown() = 0;

  /**
   * Schedule a task to run in the execution context.
   */
  virtual void schedule(Task *task) = 0;
};

/**
 * Adopted Thread RAII
 *
 * RAII class to help an executor adopt a thread.
 */
template <typename Executor>
class adopted_thread {
 public:
  explicit adopted_thread(Executor *exec_ctx)
      : exec_ctx_(exec_ctx), task_(exec_ctx_->adopt_thread()) {}
  ~adopted_thread() {
    if (task_) {
      exec_ctx_->unadopt_thread(task_);
    }
  }
  adopted_thread(adopted_thread const &) = delete;
  adopted_thread(adopted_thread &&rhs) {
    std::swap(exec_ctx_, rhs.exec_ctx_);
    std::swap(task_, rhs.task_);
  }
  // XXX: move operator
 private:
  Executor *exec_ctx_{nullptr};
  Task *task_{nullptr};
};

}  // namespace sled::executor
