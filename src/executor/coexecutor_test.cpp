/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/coexecutor.h"

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
    exec_ctx->unadopt_thread(task);
  }

  ex::CoExecutor exec_ctx;
  ex::Task *thread_task;
};

TEST_F(CoExecutorTest, schedule_task) {
  auto task = exec_ctx.create_task([]() { return 5; });
  auto f1 = task.queue_start();
  exec_ctx.resume_pending();
  EXPECT_EQ(5, f1->wait());
}

TEST_F(CoExecutorTest, task_queue_void) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() { count++; });
  auto f1 = task.queue_start();
  exec_ctx.resume_pending();
  f1->wait();
  EXPECT_EQ(1, count);
}

TEST_F(CoExecutorTest, producer_consumer_same_thread) {
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
  auto f1 = consumer.queue_start();
  auto f2 = producer.queue_start();
  f2->wait();
  f1->wait();
  EXPECT_EQ(45, total);
  exec_ctx.shutdown();
}

TEST_F(CoExecutorTest, producer_consumer) {
  std::thread thr{thread_fn, &exec_ctx};
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
  auto f1 = consumer.queue_start();
  auto f2 = producer.queue_start();
  f2->wait();
  f1->wait();
  EXPECT_EQ(45, total);
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(CoExecutorTest, producer_consumer_as_return) {
  std::thread thr{thread_fn, &exec_ctx};
  ex::Channel<int, ex::CoExecutor> channel;
  int max = 10;

  auto consumer = exec_ctx.create_task([&]() {
    int total = 0;
    for (int i = 0; i < max; i++) {
      int d = channel.get();
      total += d;
    }
    return total;
  });
  auto producer = exec_ctx.create_task([&]() {
    for (int i = 0; i < max; i++) {
      channel.put(i);
    }
  });
  auto f1 = consumer.queue_start();
  auto f2 = producer.queue_start();
  f2->wait();
  EXPECT_EQ(45, f1->wait());
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(CoExecutorTest, producer_consumer_put_wait) {
  std::thread thr{thread_fn, &exec_ctx};
  ex::Channel<int, ex::CoExecutor> channel;
  int total = 0;
  int max = 1000;

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
  auto f1 = consumer.queue_start();
  auto f2 = producer.queue_start();
  f2->wait();
  f1->wait();
  EXPECT_EQ(499500, total);
  EXPECT_EQ(0, channel.size());
  exec_ctx.shutdown();
  thr.join();
}

class MultipleCoExecutorTest : public ::testing::Test {
 protected:
  MultipleCoExecutorTest() = default;

  void SetUp() override {
    thread_task = exec_ctx1.adopt_thread();
    thr1 = std::thread{thread_fn, &exec_ctx1};
    thr2 = std::thread{thread_fn, &exec_ctx2};
  }
  void TearDown() override {
    exec_ctx1.unadopt_thread(thread_task);
    exec_ctx1.shutdown();
    thr1.join();
    exec_ctx2.shutdown();
    thr2.join();
  }

  static void thread_fn(ex::CoExecutor *exec_ctx) {
    auto task = exec_ctx->adopt_thread();
    task->run();
  }

  ex::Task *thread_task;
  ex::CoExecutor exec_ctx1;
  std::thread thr1;
  ex::CoExecutor exec_ctx2;
  std::thread thr2;
};

TEST_F(MultipleCoExecutorTest, cross_future) {
  ex::Future<int, ex::CoExecutor> fut;

  // Consumer
  auto task1 = exec_ctx1.create_task([&]() -> int { return fut.wait(); });
  auto result1 = task1.queue_start();
  auto opt1 = result1->get();
  EXPECT_FALSE(opt1.has_value());

  // Producer
  auto task2 = exec_ctx2.create_task([&]() { fut.set_result(5); });
  auto result2 = task2.queue_start();
  result2->wait();

  EXPECT_EQ(5, result1->wait());
}

TEST_F(MultipleCoExecutorTest, unique_ptr_return) {
  using task_t = ex::CoExecutor::task_t<func::function<std::unique_ptr<int>()>>;
  task_t task(&exec_ctx1, []() { return std::make_unique<int>(); });

  auto *fut = task.queue_start();

  auto intp = fut->wait();
  EXPECT_FALSE(intp == nullptr);
  *intp = 5;
  EXPECT_EQ(5, *intp.get());
}

struct AsyncReturn {
  using task_t = ex::CoExecutor::task_t<func::function<intptr_t()>>;

  AsyncReturn(ex::CoExecutor *exec_ctx, intptr_t arg)
      : task(exec_ctx, [self = this]() { return self->arg; }), arg(arg) {}

  auto queue_start() { return task.queue_start(); }

  task_t task;
  intptr_t arg{0};
};

TEST_F(MultipleCoExecutorTest, unique_ptr_struct) {
  auto callable = [&]() {
    auto ret = std::make_unique<AsyncReturn>(&exec_ctx1, 1234);
    auto fut = ret->queue_start();
    return fut->wait();
  }();

  EXPECT_EQ(1234, callable);
}
