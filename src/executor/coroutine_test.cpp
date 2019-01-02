/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/channel.h"
#include "sled/coroutine.h"
#include "sled/threadpool.h"

#include <thread>

#include "gtest/gtest.h"

namespace ex = sled::executor;

class CoExecutorTest : public ::testing::Test {
 protected:
  CoExecutorTest() = default;

  void SetUp() override { thread_task = exec_ctx.adopt_thread(); }
  void TearDown() override { exec_ctx.unadopt_thread(thread_task); }

  static void thread_fn(ex::CoExecutor *exec_ctx) {
    auto task = exec_ctx->adopt_thread();
    task->run();
  }

  ex::CoExecutor exec_ctx;
  ex::Task *thread_task;
};

TEST_F(CoExecutorTest, schedule_task) {
  auto task = exec_ctx.create_task([]() { return 5; });
  auto &f1 = task.queue_start();
  exec_ctx.resume_once();
  EXPECT_EQ(5, f1.wait());
}

TEST_F(CoExecutorTest, task_queue_void) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() { count++; });
  auto &f1 = task.queue_start();
  exec_ctx.resume_once();
  f1.wait();
  EXPECT_EQ(1, count);
}

TEST_F(CoExecutorTest, producer_consumer) {
  auto thr = std::thread{thread_fn, &exec_ctx};
  ex::Channel<int, ex::CoExecutor> channel;
  int total = 0;
  int max = 10;

  auto consumer = exec_ctx.create_task([&]() {
    for (int i = 0; i < max; i++) {
      int d = channel.get();
      total += d;
    }
  });
  auto producer = exec_ctx.create_task([&]() {
    for (int i = 0; i < max; i++) {
      channel.put(i);
    }
  });
  auto &f1 = consumer.queue_start();
  auto &f2 = producer.queue_start();
  f2.wait();
  f1.wait();
  EXPECT_EQ(45, total);
  exec_ctx.shutdown();
  thr.join();
}

//
// Low-level coroutine tests.
//

class CoroutineTest : public ::testing::Test {
 protected:
  CoroutineTest() = default;

  void SetUp() override {}
  void TearDown() override {}
};

// XXX: This is a hack
using Coroutine = sled::executor::Coroutine;

struct ThreadState {
  std::unique_ptr<Coroutine> ctx_storage;
  Coroutine *ctx{};
  stack_ctx other_ctx{};
  uint64_t data{};
};

static void test_thread(ThreadState *state) {
  // Immediately yield to get back to our main thread.
  state->ctx->yield(&state->other_ctx);

  // Now our ISR should have run, verify the changes
  EXPECT_EQ(100, state->data);

  // Now return to the original context
  state->ctx->yield(&state->other_ctx);
}

static void test_isr(uint64_t *data_ptr) {
  EXPECT_EQ(50, *data_ptr);
  *data_ptr = 100;
}

TEST_F(CoroutineTest, simple_isr) {
  ThreadState state;
  state.data = 50;

  state.ctx_storage = std::make_unique<Coroutine>(test_thread, &state);
  state.ctx = state.ctx_storage.get();

  // Switch to our test context
  state.ctx->start(&state.other_ctx);

  // We're back, check our data is okay
  EXPECT_EQ(50, state.data);

  // Now call the ISR
  state.ctx->irq(&state.other_ctx, &test_isr, &state.data);

  // Now we're back, verify the ISR did its thing
  EXPECT_EQ(100, state.data);
}

static void test_throw_thread(ThreadState *state) {
  try {
    // Immediately yield to get back to our main thread.
    state->ctx->yield(&state->other_ctx);

  } catch (std::exception &) {
    state->data = 200;
  }

  // Now return to the original context
  state->ctx->yield(&state->other_ctx);
}

static void test_throw_isr(uint64_t *data_ptr) {
  *data_ptr = 100;

  throw std::logic_error("Evil");
}

TEST_F(CoroutineTest, throw_isr) {
  ThreadState state;
  state.data = 50;

  state.ctx_storage = std::make_unique<Coroutine>(test_throw_thread, &state);
  state.ctx = state.ctx_storage.get();

  // Switch to our test context
  state.ctx->start(&state.other_ctx);

  // We're back, check our data is okay
  EXPECT_EQ(50, state.data);

  try {
    // Now call the ISR
    state.ctx->irq(&state.other_ctx, &test_throw_isr, &state.data);
  } catch (std::exception &) {
    state.data = 400;
  }

  // Now we're back, verify the exception handler did its thing
  EXPECT_EQ(200, state.data);
}

static void test_inner_throw_thread(ThreadState *state) {
  // Yield for awhile
  while (state->data == 50) {
    state->ctx->yield(&state->other_ctx);
  }

  assert(state->data == 75);

  try {
    // Now throw an exception
    test_throw_isr(&state->data);
  } catch (std::exception &) {
    state->data = 200;
  }

  // Now return to the original context
  state->ctx->yield(&state->other_ctx);
}

TEST_F(CoroutineTest, throw_inside) {
  ThreadState state;
  state.data = 50;

  state.ctx_storage =
      std::make_unique<Coroutine>(test_inner_throw_thread, &state);
  state.ctx = state.ctx_storage.get();

  // Switch to our test context
  state.ctx->start(&state.other_ctx);

  // We're back, check that our data is okay
  EXPECT_EQ(50, state.data);

  // Tell the other thread to throw
  state.data = 75;
  state.ctx->resume(&state.other_ctx);

  // Now we're back, verify the exception handler did its thing
  EXPECT_EQ(200, state.data);
}
