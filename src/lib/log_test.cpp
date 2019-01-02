/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/log.h"

#include "gtest/gtest.h"

class LogTest : public ::testing::Test {
 protected:
  LogTest() : logman(logstream) {}

  void SetUp() override {}

  void TearDown() override {}

  std::stringstream logstream;
  sled::log::LoggingManager logman;
};

TEST_F(LogTest, facility_test) {
  auto f1{sled::log::Facility::None};
  EXPECT_EQ(0, f1.v);
  auto f2{sled::log::Facility::Util};
  EXPECT_EQ(1, f2.v);
  auto f3{sled::log::Facility::Exec};
  EXPECT_EQ(2, f3.v);
  auto f4{sled::log::Facility::Perf};
  EXPECT_EQ(3, f4.v);
}

TEST_F(LogTest, severity_test) {
  EXPECT_LT(sled::log::Severity::Trace, sled::log::Severity::Debug);
}

using severity = sled::log::Severity;
using facility = sled::log::Facility;

TEST_F(LogTest, message_construct) {
  sled::log::stream_message msg(facility::Test, severity::Trace);

  msg.msg() << "Hello World!";

  EXPECT_EQ("Hello World!", msg.what());
}

TEST_F(LogTest, facility_name) {
  std::stringstream ss;

  ss << facility::None;
  EXPECT_EQ("None", ss.str());
}

TEST_F(LogTest, log_always) {
  logman.log_always(facility::Exec, severity::Warning, "Hello World!");

  EXPECT_EQ("Hello World!\n", logstream.str());
}

TEST_F(LogTest, add_facilty) {
  facility facility1 = logman.add_facility("facility1");

  EXPECT_LT(facility::User3.v, facility1.v);
}
