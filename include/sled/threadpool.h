/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "sled/executor.h"

namespace sled::executor {

/**
 * ThreadPool Executor.
 *
 * Threadpool based executor. Each task is ran on an available thread.  Threads
 * are manually managed by the caller.
 *
 * Example:
 * TpExecutor exec_ctx;
 * auto thread_fn = [](auto *exec_ctx) {
 *  auto thread_task = exec_ctx->adopt_thread();
 *  thread_task->run();
 * };
 * auto thr1 = std::thread{thread_fn, &exec_ctx};
 * ...
 */
class TpExecutor final : public Executor {
 public:
  /**
   * Executor task type.
   */
  template <typename closure_t>
  using task_t = sled::executor::ExecTask<TpExecutor, closure_t>;

 public:
  TpExecutor();
  ~TpExecutor() final;

  static void yield();
  static sled::executor::Task *cur_task();
  static sled::executor::TaskId current_task_id();
  static void cur_task_suspend();

  template <typename Fn>
  task_t<Fn> create_task(Fn &&fn) {
    return task_t<Fn>(this, std::move(fn));
  }

  /**
   * Yield the current task, allowing another nonblocking task
   * to resume.
   */
  // XXX: static void yield();

  /**
   * Retrieve the current executing task.
   */
  // XXX: static Task *cur_task();

  Task *adopt_thread();
  void unadopt_thread(Task *task);
  void resume();
  void resume_once();
  void run();
  void shutdown();
  void schedule(sled::executor::Task *task);

  /**
   * Return the next runnable task.
   */
  std::optional<Task *> next();

 private:
  /**
   * TpExecutor Thread Task.
   *
   * TpThreadTask handles the internals of managing the executor.
   * It's created by calling the corresponding adopt thread function.
   */
  class TpThreadTask final : public Task {
   public:
    TpThreadTask(TpExecutor *exec_ctx) : exec_ctx_(exec_ctx) {}
    ~TpThreadTask() { TpExecutor::current_task_ = nullptr; }

    void run() override {
      try {
        for (;;) {
          if (auto task_opt = exec_ctx_->next(); task_opt.has_value()) {
            auto *task = task_opt.value();
            task->run();
          } else {
            break;
          }
        }
      } catch (std::exception &) {
      }
    }
    void suspend() override {}
    void wake() override {}
    void schedule() override {}

   private:
    void yield_internal() override {}

    TpExecutor *exec_ctx_;
  };

  thread_local static sled::executor::Task *current_task_;
  SyncChannel<sled::executor::Task *> runnable_;
};

}  // namespace sled::executor
