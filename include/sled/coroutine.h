/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"

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

}  // namespace sled::executor
