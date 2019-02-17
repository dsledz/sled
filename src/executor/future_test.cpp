/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/future.h"

#include "gtest/gtest.h"

#include "executor_mock.h"

namespace ex = sled::executor;

template <typename result_t>
using future_t = ex::Future<result_t, MockExecutor>;

class MockedFutureTest : public MockedExecutorTest {
 protected:
  using MockedExecutorTest::MockedExecutorTest;
};

TEST_F(MockedFutureTest, construct) {
  future_t<int> f1{};
  future_t<int> f2{5};
  EXPECT_FALSE(f1.valid());
  EXPECT_TRUE(f2.valid());
}

TEST_F(MockedFutureTest, mock_execute_async) {
  auto task = exec_ctx.create_task([]() { return 5; });
  auto f1 = task.execute_async();
  EXPECT_TRUE(f1->valid());
  EXPECT_EQ(5, f1->get().value());
}

#ifndef WIN32
template <typename executor_t, typename Fn>
struct task_wrapper {
  task_wrapper(executor_t *exec_ctx, Fn &&fn)
      : task(exec_ctx->create_task(std::forward<Fn>(fn))) {}

  typename executor_t::template task_t<Fn> task;
};

TEST_F(MockedFutureTest, wrapped_execute_async) {
  task_wrapper wrapper(&exec_ctx, []() { return 5; });
  auto f1 = wrapper.task.execute_async();
  EXPECT_TRUE(f1->valid());
  EXPECT_EQ(5, f1->get().value());
}
#endif

TEST_F(MockedFutureTest, get_valid) {
  future_t<int> f1{};
  auto no_val = f1.get();
  EXPECT_FALSE(no_val.has_value());
  f1.set_result(5);
  auto val = f1.get();
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(5, val.value());
}

TEST_F(MockedFutureTest, wait_valid) {
  future_t<int> f1{};
  EXPECT_FALSE(f1.valid());
  f1.set_result(5);
  auto val = f1.wait();
  EXPECT_EQ(5, val);
}

struct non_copyable {
  non_copyable(uint64_t a, uint64_t b) : a(a), b(b) {}

  // Copy disabled
  non_copyable(non_copyable const &rhs) = delete;
  non_copyable &operator=(non_copyable const &rhs) = delete;
  // copy enabled
  non_copyable(non_copyable &&rhs) = default;
  non_copyable &operator=(non_copyable &&rhs) = default;

  bool operator==(non_copyable const &rhs) const noexcept {
    return (a == rhs.a) && (b == rhs.b);
  }

  uint64_t a{0};
  uint64_t b{0};
};

TEST_F(MockedFutureTest, non_copy_result) {
  future_t<non_copyable> f1{};
  f1.set_result(non_copyable{1, 2});
  non_copyable expected{1, 2};
  non_copyable actual{f1.wait()};
  EXPECT_EQ(expected, actual);
}

struct non_moveable {
  non_moveable(uint64_t a, uint64_t b) : a(a), b(b) {}

  // Copy is enabled
  non_moveable(non_moveable const &rhs) = default;
  non_moveable &operator=(non_moveable const &rhs) = default;
  // Move is disabled
  non_moveable(non_moveable &&rhs) = delete;
  non_moveable &operator=(non_moveable &&rhs) = delete;

  bool operator==(non_moveable const &rhs) const noexcept {
    return (a == rhs.a) && (b == rhs.b);
  }

  uint64_t a{0};
  uint64_t b{0};
};

#ifndef WIN32
TEST_F(MockedFutureTest, non_moveable) {
  future_t<non_moveable> f1{};
  f1.set_result(non_moveable{1, 2});
  non_moveable expected{1, 2};
  non_moveable actual{f1.wait()};
  EXPECT_EQ(expected, actual);
}
#endif

TEST_F(MockedFutureTest, void_return) {
  future_t<void> f1{};
  f1.set_result();
  f1.wait();
  // Success is the lack of a hang
}
