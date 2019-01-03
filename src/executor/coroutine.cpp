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

} // namespace sled::executor
