/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/channel.h"
#include "sled/enum.h"
#include "sled/future.h"
#include "sled/ident.h"
#include "sled/lock.h"
#include "sled/type_traits.h"

#include <functional>

namespace sled::executor {

/**
 * Task flags.
 */
enum class TaskFlag {
  Running = 0x01,   /**< Running */
  Finished = 0x02,  /**< Finished */
  Canceled = 0x04,  /**< Canceled */
  Dead = 0x08,      /**< Dead */
  Queued = 0x10,    /**< Queued */
  Suspended = 0x20, /**< Suspended */
};

using TaskFlags = sled::flags<TaskFlag>;

/**
 * Task identifier.
 *
 * An auto-incrementing value that uniquely defines a task.  Does not support
 * overflowke
 */
struct TaskId {
  uint64_t id{0};
  bool operator==(TaskId const &rhs) { return id == rhs.id; }

  static TaskId next() {
    static std::atomic<uint64_t> next_id = ATOMIC_VAR_INIT(1);
    return {std::atomic_fetch_add(&next_id, static_cast<uint64_t>(1))};
  }
};

/**
 * Type-erased Executor task.
 *
 * Implements a type-erased executor task.
 */
class Task {
 public:
  using task_fn = func::function<void() noexcept>;

  Task() : flags_(), ident_() {}

  virtual ~Task() = default;
  Task(const Task &task) = delete;

  /**
   * Identity
   *
   * Returns the detailed task identity for logging and debugging.
   */
  sled::ident const &ident() const { return ident_; }

  /**
   * Id.
   *
   * @returns task id.
   */
  TaskId id() const { return {ident_.id()}; }

  /**
   * Execute the actual task.  This call is done within the exec_ctx.
   */
  virtual void run() = 0;

  /**
   * Suspend the task until it can be woken up.  This call can only be called
   * from within the task and the associated exec_ctx.
   */
  virtual void suspend() = 0;

  /**
   * Resume a suspended task.  This call can only be called from outside the
   * task.
   */
  virtual void wake() = 0;

  /**
   * Schedule a task to run.
   */
  virtual void schedule() = 0;

  /**
   * Yield allowing other tasks to run.
   */
  virtual void yield() = 0;

  /**
   * Schedule the task on the specified channel.
   */
  // XXX: void schedule(TaskChannel *channel);

  // XXX: friend std::ostream &operator<<(std::ostream &os, const Task &t);

 protected:
  /**
   * Debug logging.
   */
  // XXX: void log(LogLevel level, const std::string &fmt);

  TaskFlags flags_;
  sled::ident ident_;
};

/**
 * Task specialization that contains a result
 */
template <typename result_t>
class ResultTask : public Task {
 protected:
  std::optional<result_t> result_;
};

/**
 * Task specialization that does not contain a result.
 */
class VoidTask : public Task {
 protected:
  std::optional<int> result_;
};

template <typename Fn>
using task_type_t = typename std::conditional<
    std::is_same<void, typename function_traits<Fn>::result_type>::value,
    VoidTask, ResultTask<typename function_traits<Fn>::result_type>>::type;

/**
 * Executor specialized task
 */
template <class executor_t, typename Fn>
class ExecTask : public task_type_t<Fn> {
 public:
  using result_t = typename function_traits<Fn>::result_type;
  using future_t = sled::executor::Future<result_t, executor_t>;

  ExecTask(executor_t *exec_ctx, Fn closure)
      : task_type_t<Fn>(), exec_ctx_(exec_ctx), closure_(closure) {}
  ExecTask(ExecTask const &) = delete;
  ExecTask(ExecTask &&rhs) = default;

  void run() final {
    if constexpr (std::is_same<void, result_t>::value) {
      closure_();
      future_.set_result();
    } else {
      auto result = closure_();
      future_.set_result(result);
    }
    // We can no longer touch task after sharing the result
  }
  void wake() final {}
  void suspend() final {}
  void yield() final {}
  void schedule() final {}

  auto execute() { return closure_(); }

  future_t *execute_async() {
    if constexpr (std::is_same<void, result_t>::value) {
      closure_();
      future_.set_result();
    } else {
      auto result = closure_();
      future_.set_result(result);
    }
    return &future_;
  }

  /**
   * Queue task to start.
   */
  future_t *queue_start() {
    future_t *f = &future_;
    exec_ctx_->schedule(this);
    return f;
  }

 private:
  executor_t *exec_ctx_;
  future_t future_;
  Fn closure_;
};

}  // namespace sled::executor
