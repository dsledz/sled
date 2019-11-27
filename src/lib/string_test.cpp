/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/string.h"

#include "gtest/gtest.h"

namespace sled {

class StringTest : public ::testing::Test {
 protected:
  StringTest() = default;
};

template <typename Fn>
std::string call(Fn &&fn, const std::string &str) {
  std::string tmp{str};
  fn(tmp);
  return tmp;
}

TEST_F(StringTest, trim_test) {
  EXPECT_EQ("blah", call(ltrim, "   blah"));
  EXPECT_EQ("blah", call(ltrim, "\nblah"));
  EXPECT_EQ("blah", call(ltrim, "\tblah"));
  EXPECT_EQ("blah", call(rtrim, "blah "));
  EXPECT_EQ("blah", call(rtrim, "blah\n"));
  EXPECT_EQ("blah", call(rtrim, "blah\t"));
  EXPECT_EQ("blah", call(trim, " blah "));
  EXPECT_EQ("blah", call(trim, "\nblah\n"));
  EXPECT_EQ("blah", call(trim, "\tblah\t"));
}

TEST_F(StringTest, tolower_test) {
  EXPECT_EQ("blah", str_tolower_copy("BLaH"));
}

TEST_F(StringTest, ends_with_test) {
  EXPECT_TRUE(ends_with("The quick brown fox", "fox"));
  EXPECT_FALSE(ends_with("The quick brown fox", "wolf"));
  EXPECT_FALSE(ends_with("The quick brown fox", "brown"));
}

}  // namespace sled
