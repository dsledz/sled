/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/mutex.h"

#include "gtest/gtest.h"

#include "executor_mock.h"

namespace ex = sled::executor;
namespace sync = sled::sync;

class MockedMutexTest : public MockedExecutorTest {
 protected:
  using MockedExecutorTest::MockedExecutorTest;
};

TEST_F(MockedMutexTest, construct) {
  sync::mutex mtx;

  EXPECT_FALSE(mtx.owner() == exec_ctx.current_task_id());
}
