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
  auto f1 = consumer.queue_start();
  auto f2 = producer.queue_start();
  f2->wait();
  f1->wait();
  EXPECT_EQ(45, total);
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(CoExecutorTest, producer_consumer_as_return) {
  auto thr = std::thread{thread_fn, &exec_ctx};
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
  auto thr = std::thread{thread_fn, &exec_ctx};
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
  exec_ctx.shutdown();
  thr.join();
}


