/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/threadpool.h"

namespace sled::executor {

thread_local Task* TpExecutor::current_task_{nullptr};

TpExecutor::TpExecutor() = default;
TpExecutor::~TpExecutor() { debug_assert(runnable_.empty()); }

Task* TpExecutor::cur_task() { return current_task_; }
TaskId TpExecutor::current_task_id() { return cur_task()->id(); }
void TpExecutor::cur_task_suspend() {}

Task* TpExecutor::adopt_thread() {
  auto task = std::make_unique<TpThreadTask>(this);
  TpExecutor::current_task_ = task.get();
  auto base_task = std::unique_ptr<Task>(std::move(task));
  return base_task.release();
}

void TpExecutor::unadopt_thread(Task* task) {
  TpExecutor::current_task_ = nullptr;
}

void TpExecutor::resume() {}

void TpExecutor::resume_pending() {
  if (auto task_opt = next(); task_opt.has_value()) {
    auto* task = task_opt.value();
    task->run();
  }
}

void TpExecutor::run() {}
void TpExecutor::shutdown() { runnable_.close(); }
void TpExecutor::schedule(sled::executor::Task* task) { runnable_.put(task); }

std::optional<Task*> TpExecutor::next() { return runnable_.get(); }

}  // namespace sled::executor
