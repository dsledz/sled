/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "executor_mock.h"

thread_local sled::executor::Task* MockExecutor::current_task_{nullptr};

MockExecutor::MockExecutor() = default;
MockExecutor::~MockExecutor() = default;

void MockExecutor::yield() {}

sled::executor::Task* MockExecutor::cur_task() { return current_task_; }

sled::executor::TaskId MockExecutor::current_task_id() {
  return cur_task()->id();
}

void MockExecutor::cur_task_suspend() {}

sled::executor::Task* MockExecutor::adopt_thread() {
  sled::executor::Task* task = nullptr;
  MockExecutor::current_task_ = task;
  return task;
}

void MockExecutor::unadopt_thread(sled::executor::Task* task) {
  MockExecutor::current_task_ = nullptr;
}

void MockExecutor::resume() {
  while (!runnable_.empty()) {
    auto* task = runnable_.front();
    runnable_.pop_front();
    task->run();
  }
}

void MockExecutor::resume_once() {
  if (!runnable_.empty()) {
    auto* task = runnable_.front();
    runnable_.pop_front();
    task->run();
  }
}

void MockExecutor::run() {}
void MockExecutor::shutdown() {}

void MockExecutor::schedule(sled::executor::Task* task) {
  runnable_.push_back(task);
}
