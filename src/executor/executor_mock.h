/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/executor.h"
#include "sled/future.h"

#include "gtest/gtest.h"

#include <deque>

/**
 * Mock executor interface for testing.
 */
struct MockExecutor : public sled::executor::Executor {
 public:
  /**
   * Executor task type.
   */
  template <typename closure_t>
  using task_t = sled::executor::ExecTask<MockExecutor, closure_t>;

  class MockThreadTask final : public sled::executor::Task {
   public:
    MockThreadTask(MockExecutor *exec_ctx) : exec_ctx_(exec_ctx) {
      assert(current_task_ == nullptr);
    }
    ~MockThreadTask() {
      assert(current_task_ == this);
      MockExecutor::current_task_ = nullptr;
    }

    void run() override {}
    void suspend() override {}
    void wake() override {}
    void schedule() override {}
    void yield() override {}

   private:
    MockExecutor *exec_ctx_;
  };

 public:
  MockExecutor();
  ~MockExecutor() final;

  static void yield();
  static sled::executor::Task *cur_task();
  static sled::executor::TaskId current_task_id();
  static void cur_task_suspend();

  template <typename Fn>
  task_t<Fn> create_task(Fn &&fn) {
    return task_t<Fn>(this, std::forward<Fn>(fn));
  }

  sled::executor::Task *adopt_thread() final;
  void unadopt_thread(sled::executor::Task *task) final;
  void resume() final;
  void resume_pending() final;
  void run() final;
  void shutdown() final;
  void schedule(sled::executor::Task *task) final;

 private:
  thread_local static sled::executor::Task *current_task_;
  std::mutex mtx_;
  std::deque<sled::executor::Task *> runnable_;
};

class MockedExecutorTest : public ::testing::Test {
 protected:
  MockedExecutorTest() = default;

  void SetUp() override {
    thread_task = exec_ctx.adopt_thread();
    assert(thread_task != nullptr);
    assert(exec_ctx.cur_task() != nullptr);
  }

  void TearDown() override { exec_ctx.unadopt_thread(thread_task); }

  MockExecutor exec_ctx;
  sled::executor::Task *thread_task;
};
