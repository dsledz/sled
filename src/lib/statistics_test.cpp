/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/statistics.h"

#include "gtest/gtest.h"

// XXX: eww
using namespace sled;

struct TestStats : public StatsImpl {
  Counter<int> StatA{0};
  Aggregate<int> StatB{};
  Counter<int> StatC{0};

  void report_stats(StatisticsReporter &reporter) final {
    reporter.add("StatA", StatA);
    reporter.add("StatB", StatB);
    reporter.add("StatC", StatC);
  }
};

class StatisticsTest : public ::testing::Test {
 protected:
  StatisticsTest() = default;

  StatisticsReport report;
  TestStats stats;
};

TEST_F(StatisticsTest, report) {
  stats.StatA = 2;
  stats.StatB.s = 100;
  stats.StatB.c = 10;
  stats.StatC = 50;

  stats.report_stats(report);

  EXPECT_EQ(2, report.get("StatA"));
  EXPECT_EQ(10.0, report.get("StatB"));
  EXPECT_EQ(50, report.get("StatC"));
}

TEST_F(StatisticsTest, increment) {
  stats.StatA += 2;
  for (int i = 0; i < 10; i++) {
    stats.StatB += 10;
  }
  stats.StatC += 50;

  stats.report_stats(report);

  EXPECT_EQ(2, report.get("StatA"));
  EXPECT_EQ(10.0, report.get("StatB"));
  EXPECT_EQ(50, report.get("StatC"));
}
