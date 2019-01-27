/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/executor.h"

#include "gtest/gtest.h"

#include "executor_mock.h"

namespace ex = sled::executor;

class MockedTaskTest : public MockedExecutorTest {
 protected:
  using MockedExecutorTest::MockedExecutorTest;
};

TEST_F(MockedTaskTest, execute) {
  auto task = exec_ctx.create_task([]() { return 5; });
  auto val = task.execute();
  EXPECT_EQ(5, val);
}

TEST_F(MockedTaskTest, schedule_task) {
  auto task = exec_ctx.create_task([]() { return 5; });
  auto f1 = task.queue_start();
  exec_ctx.resume();
  EXPECT_EQ(5, f1->wait());
}

TEST_F(MockedTaskTest, void_return) {
  int count = 0;
  auto task = exec_ctx.create_task([&]() { count++; });
  auto f1 = task.queue_start();
  exec_ctx.resume();
  f1->wait();
  EXPECT_EQ(1, count);
}

TEST_F(MockedTaskTest, multiple_tasks) {
  auto task1 = exec_ctx.create_task([]() { return 1; });
  auto task2 = exec_ctx.create_task([]() { return 2; });
  auto task3 = exec_ctx.create_task([]() { return 3; });
  auto task4 = exec_ctx.create_task([]() { return 4; });
  auto f1 = task1.queue_start();
  auto f2 = task2.queue_start();
  auto f3 = task3.queue_start();
  auto f4 = task4.queue_start();
  exec_ctx.resume_pending();
  EXPECT_TRUE(f1->valid());
  EXPECT_FALSE(f2->valid());
  exec_ctx.resume_pending();
  EXPECT_TRUE(f2->valid());
  EXPECT_FALSE(f3->valid());
  exec_ctx.resume();
  EXPECT_TRUE(f3->valid());
  EXPECT_TRUE(f4->valid());
}

#if 0
TEST_F(MockedTaskTest, create_strand) {
  auto strand = exec_ctx.create_strand();
}
#endif
