/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/channel.h"
#include "sled/executor.h"

#include "gtest/gtest.h"

#include "executor_mock.h"

namespace ex = sled::executor;

template <typename result_t>
using channel_t = ex::Channel<result_t, MockExecutor>;

class MockedChannelTest : public MockedExecutorTest {
 protected:
  using MockedExecutorTest::MockedExecutorTest;
};

TEST_F(MockedChannelTest, construct) {
  channel_t<int> ch;
  EXPECT_EQ(0, ch.size());
}

TEST_F(MockedChannelTest, put_get) {
  channel_t<int> ch;
  ch.put(3);
  EXPECT_EQ(1, ch.size());
  EXPECT_EQ(3, ch.get());
}
