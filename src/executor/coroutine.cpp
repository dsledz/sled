/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/coroutine.h"
#include "sled/channel.h"

#ifndef WIN32
#include <valgrind/valgrind.h>
#else
#define VALGRIND_STACK_REGISTER(...) 0
#define VALGRIND_STACK_DEREGISTER(...)
#endif

namespace sled::executor {

void Coroutine::build_stack(intptr_t rip) {
  valgrind_stack =
      VALGRIND_STACK_REGISTER(m_stack.data(), m_stack.data() + m_stack.size());

  // 128 byte red zone
  uint8_t *stack = &(*(m_stack.end() - 128));
  // 8 bytes for our hazard ebp
  stack -= 8;
  *reinterpret_cast<uint64_t *>(stack) = 0xFFFFFFFFFFFFFFFFul;
  // 8 bytes for our hazard ip
  stack -= 8;
  *reinterpret_cast<uint64_t *>(stack) = 0xFFFFFFFFFFFFFFFFul;
  // 8 bytes for our real ebp
  stack -= 8;
  *reinterpret_cast<uint64_t *>(stack) = reinterpret_cast<uintptr_t>(stack);
  // 8 bytes for our real eip
  stack -= 8;
  *reinterpret_cast<uint64_t *>(stack) = rip;
#ifndef WIN32
  // 40 bytes for our registers + 8 for rip
  stack -= 48;
#endif
  m_ctx.rsp = reinterpret_cast<uintptr_t>(stack);
}

Coroutine::~Coroutine() {
  /* TODO(dan): Assert we're not on that stack */
  VALGRIND_STACK_DEREGISTER(valgrind_stack);
}

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

void CoExecutor::resume_once() {
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

}  // namespace sled::executor
