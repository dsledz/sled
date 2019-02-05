/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/coexecutor.h"

namespace sled::executor {

//
// CoExecutor
//

thread_local Task *CoExecutor::current_task_{nullptr};

CoExecutor::CoExecutor() = default;
CoExecutor::~CoExecutor() = default;

Task *CoExecutor::cur_task() { return current_task_; }
TaskId CoExecutor::current_task_id() { return cur_task()->id(); }
void CoExecutor::cur_task_suspend() { cur_task()->suspend(); }

Task *CoExecutor::adopt_thread() {
  assert(CoExecutor::current_task_ == nullptr);
  auto task = std::make_unique<CoThreadTask>(this);
  CoExecutor::current_task_ = task.get();
  auto base_task = std::unique_ptr<Task>(std::move(task));
  return base_task.release();
}

void CoExecutor::unadopt_thread(Task *task) {
  CoExecutor::current_task_ = nullptr;
}

void CoExecutor::yield() {
  // auto task = CoExecutor::current_task_;
}

void CoExecutor::resume() {
  debug_assert(CoExecutor::current_task_ != nullptr);
  // Resume our CoThreadTask to do the hard work.
  CoExecutor::current_task_->run();
}

void CoExecutor::resume_pending() {
  debug_assert(CoExecutor::current_task_ != nullptr);
  if (auto task_opt = runnable_.try_get(); task_opt.has_value()) {
    auto *task = task_opt.value();
    CoExecutor::current_task_ = task;
    task->run();
  }
}

void CoExecutor::run() {}
void CoExecutor::shutdown() { runnable_.close(); }
void CoExecutor::schedule(sled::executor::Task *task) { runnable_.put(task); }

std::optional<Task *> CoExecutor::next() { return runnable_.get(); }
std::optional<Task *> CoExecutor::try_next() { return runnable_.try_get(); }

}  // namespace sled::executor
