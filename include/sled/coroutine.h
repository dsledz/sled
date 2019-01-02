/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/executor.h"
#include "sled/lock.h"
#include "sled/platform.h"

#include <deque>
#include <vector>

namespace sled::executor {

/**
 * Coroutine Context.
 *
 * Context which holds stack and thread stack.
 */
class Coroutine {
 public:
  template <class T>
  explicit Coroutine(void (*co_fn)(T *), T *co_data = nullptr)
      : m_stack(64 * 1024),
        m_data(reinterpret_cast<intptr_t>(co_data)),
        m_ctx() {
    auto fn = reinterpret_cast<intptr_t>(co_fn);
    build_stack(fn);
  }
  ~Coroutine();

  /**
   * Yield current coroutine to @restore_context.
   */
  a_forceinline void yield(stack_ctx *restore_context) {
    // Yield to @new_context. Execution will continue in the other context.
    // Eventually, this function will return and we can resume execution.
    SwitchContext(&m_ctx, restore_context);
  }

  /**
   * Resume yielded coroutine, storing current context to @a save_context.
   */
  a_forceinline void resume(stack_ctx *save_context) {
    // Resume a previously suspended context.  The current context information
    // will be stored.  The new context *must* eventually yield.
    SwitchContext(save_context, &m_ctx);
  }

  /**
   * Start coroutine, storing current context to @a save_context.
   */
  a_forceinline void start(stack_ctx *save_context) {
    // Start execution of this context for the first time.  Similar to @resume.
    InitialSwitchContext(save_context, &m_ctx, m_data);
  }

  /**
   * Resume yield coroutine, storing current context to @a save_context.
   */
  template <class T>
  a_forceinline void irq(stack_ctx *save_context, void (*isr_fn)(T *),
                         T *isr_data) {
    auto data = reinterpret_cast<intptr_t>(isr_data);
    auto fn = reinterpret_cast<intptr_t>(isr_fn);
    SwitchContextIrq(save_context, &m_ctx, fn, data);
  }

  stack_ctx &context() { return m_ctx; }

 private:
  void build_stack(intptr_t rip);

  std::vector<uint8_t> m_stack;
  uint64_t m_data;
  stack_ctx m_ctx;
  int valgrind_stack;
};

/**
 * Executor task for coroutine.
 */
template <class executor_t, typename Fn>
class CoTask : public task_type_t<Fn> {
 public:
  using result_t = typename function_traits<Fn>::result_type;
  using future_t = sled::executor::Future<result_t, executor_t>;

  CoTask(executor_t *exec_ctx, Fn closure)
      : task_type_t<Fn>(),
        exec_ctx_(exec_ctx),
        co_ctx_(co_enter, this),
        closure_(closure) {}
  CoTask(CoTask const &) = delete;

  void run() final {
    // Ran in the context of CoThreadTask, potentially entered multiple times.
    debug_assert(this == executor_t::cur_task());
    debug_assert(flags_.is_set(TaskFlag::Queued));
    flags_.clear(TaskFlag::Queued);
    // Resume/start our fiber if we're not finished.
    // Due to races, it's possible the task will be queued while running.
    // If the task exits, it will run one more time.
    if (flags_.is_clear(TaskFlag::Finished)) {
      if (flags_.is_clear(TaskFlag::Running)) {
        // Switch to the fiber thread and start execution
        flags_.set(TaskFlag::Running);
        co_ctx_.start(&other_ctx_);
      } else {
        // Switch to the fiber thread. Fiber will resume after the yield call
        co_ctx_.resume(&other_ctx_);
      }
      debug_assert(flags_.is_clear(TaskFlag::Running));
    }
    // Now that we're back, we can share our result.
    // Ensure the task hasn't been re-queued.
    // We had to wait because once we set the result, we could be freed.
    if (flags_.is_set(TaskFlag::Finished) &&
        flags_.is_clear(TaskFlag::Queued)) {
      if constexpr (std::is_same<void, result_t>::value) {
        future_.set_result();
      } else {
        // XXX: Use std::optional<>::swap()
        // future_.set_result(std::move(result_.value()));
        future_.set_result(5);
      }
      // Can't touch members.
    }
  }
  void wake() final {
    if (flags_.is_clear(TaskFlag::Queued)) {
      flags_.update({TaskFlag::Queued}, {});
      exec_ctx_->schedule(this);
    }
  }
  void suspend() final {
    flags_.update({TaskFlag::Suspended}, {TaskFlag::Running});
    co_ctx_.yield(&other_ctx_);
  }

  void yield_internal() final {}
  void schedule() final {}

  auto execute() {
    // XXX: This is broken.
    return closure_();
  }

  future_t &execute_async() {
    // XXX: This is broken.
    co_enter(this);
    return future_;
  }

  /**
   * Queue task to start.
   */
  future_t &queue_start() {
    future_t &f = future_;
    flags_.set(TaskFlag::Queued);
    exec_ctx_->schedule(this);
    return f;
  }

 private:
  static void co_enter(CoTask *task) {
    // Executed in our coroutine context. Treat this as linear
    if constexpr (std::is_same<void, result_t>::value) {
      // Handle coroutines that return void
      task->closure_();
    } else {
      // Handle coroutines that return a value
      task->result_ = task->closure_();
    }
    task->flags_.update({TaskFlag::Finished}, {TaskFlag::Running});
    // We're done, switch back in a loop as we can be scheduled multiple times.
    for (;;) {
      task->co_ctx_.yield(&task->other_ctx_);
    }
  }

  TaskFlags flags_;
  executor_t *exec_ctx_;
  Coroutine co_ctx_;
  stack_ctx other_ctx_;
  future_t future_;
  Fn closure_;
};

/**
 * Coroutine executor.
 *
 * Coroutine based executor.
 */
class CoExecutor : public Executor {
 public:
  /**
   * Executor task type.
   */
  template <typename closure_t>
  using task_t = sled::executor::CoTask<CoExecutor, closure_t>;

 public:
  CoExecutor();
  ~CoExecutor() final;

  static void yield();
  static sled::executor::Task *cur_task();
  static sled::executor::TaskId current_task_id();
  static void cur_task_suspend();

  template <typename Fn>
  task_t<Fn> create_task(Fn &&fn) {
    return task_t<Fn>(this, std::move(fn));
  }

  Task *adopt_thread() final;
  void unadopt_thread(Task *task) final;
  void resume() final;
  void resume_once() final;
  void run() final;
  void shutdown() final;
  void schedule(sled::executor::Task *task) final;

  /**
   * Return the next runnable task.
   */
  std::optional<Task *> next();

 private:
  /**
   * CoExecutor Thread Task.
   *
   * CoThreadTask handles the internals of managing the executor.
   * It's created by calling the corresponding adopt thread function.
   */
  class CoThreadTask final : public Task {
   public:
    CoThreadTask(CoExecutor *exec_ctx) : exec_ctx_(exec_ctx) {}
    ~CoThreadTask() {
      // XXX: assert(current_task_ == this);
      CoExecutor::current_task_ = nullptr;
    }

    void run() override {
      try {
        for (;;) {
          CoExecutor::current_task_ = this;
          if (auto task_opt = exec_ctx_->next(); task_opt.has_value()) {
            auto *task = task_opt.value();
            CoExecutor::current_task_ = task;
            task->run();
          } else {
            break;
          }
        }
      } catch (std::exception &) {
      }
      CoExecutor::current_task_ = this;
    }

    void suspend() override {
      sled::sync::lock_guard lock(mtx_);
      // XXX: Loop until ready
      lock.wait(cv_);
    }
    void wake() override {
      sled::sync::lock_guard lock(mtx_);
      cv_.notify_all();
    }
    void schedule() override {
      // XXX: Unsure if this is really required.
    }

   private:
    void yield_internal() override {}

    std::mutex mtx_;
    std::condition_variable cv_;
    CoExecutor *exec_ctx_;
  };

  thread_local static sled::executor::Task *current_task_;
  SyncChannel<sled::executor::Task *> runnable_;
};

}  // namespace sled::executor
