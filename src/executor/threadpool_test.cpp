/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/threadpool.h"

#include "gtest/gtest.h"

#include <thread>

namespace ex = sled::executor;

class TpExecutorTest : public ::testing::Test {
 protected:
  TpExecutorTest() = default;

  void SetUp() override { thread_task = exec_ctx.adopt_thread(); }

  void TearDown() override { exec_ctx.unadopt_thread(thread_task); }

  static void thread_fn(ex::TpExecutor *exec_ctx) {
    auto task = exec_ctx->adopt_thread();
    task->run();
  }

  ex::TpExecutor exec_ctx;
  ex::Task *thread_task;
};

TEST_F(TpExecutorTest, empty_thread) {
  auto thr = std::thread{thread_fn, &exec_ctx};
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(TpExecutorTest, task_queued_after_thread) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() {
    count++;
    return true;
  });
  auto thr = std::thread{thread_fn, &exec_ctx};
  auto &f1 = task.queue_start();
  EXPECT_EQ(true, f1.wait());
  EXPECT_EQ(1, count);
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(TpExecutorTest, task_queued_after_thread_void) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() { count++; });
  auto thr = std::thread{thread_fn, &exec_ctx};
  auto &f1 = task.queue_start();
  f1.wait();
  EXPECT_EQ(1, count);
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(TpExecutorTest, task_queued_before_thread) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() {
    count++;
    return true;
  });
  auto &f1 = task.queue_start();
  auto thr = std::thread{thread_fn, &exec_ctx};
  EXPECT_EQ(true, f1.wait());
  EXPECT_EQ(1, count);
  exec_ctx.shutdown();
  thr.join();
}

TEST_F(TpExecutorTest, task_queued_same_thread) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() {
    count++;
    return true;
  });
  auto &f1 = task.queue_start();
  exec_ctx.resume_once();
  EXPECT_EQ(true, f1.wait());
  EXPECT_EQ(1, count);
  exec_ctx.shutdown();
}

TEST_F(TpExecutorTest, multiple_threads) {
  std::vector<std::unique_ptr<std::thread>> threads;
  for (int i = 0; i < 5; i++) {
    threads.push_back(std::make_unique<std::thread>(thread_fn, &exec_ctx));
  }
  exec_ctx.shutdown();
  for (auto &thr : threads) {
    thr->join();
  }
}
