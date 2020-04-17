/*
 * Copyright (c) 2020, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/cmdline.h"

#include "gtest/gtest.h"

class CmdlineTest : public ::testing::Test {
 protected:
  CmdlineTest() = default;

  void SetUp() override {}

  void TearDown() override {}

  sled::cli::tokenizer clt;
  sled::cli::parser clp;
};

TEST_F(CmdlineTest, clt_argc_argv) {
  int argc = 4;
  const char *argv[4] = {"cmd.exe", "--arg1", "hello world", "--arg2=hello"};
  auto tokens = clt.tokenize(argc, argv);
  EXPECT_EQ(3, tokens.size());
  EXPECT_EQ("cmd.exe", tokens[0].tok);
  EXPECT_EQ("--arg1", tokens[1].tok);
  EXPECT_EQ("hello world", tokens[1].opt.value());
  EXPECT_EQ("--arg2", tokens[2].tok);
  EXPECT_EQ("hello", tokens[2].opt.value());
}

TEST_F(CmdlineTest, clt_simple_tokens) {
  auto tokens = clt.tokenize("hello world");
  EXPECT_EQ(2, tokens.size());
  EXPECT_EQ("hello", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("world", tokens[1].tok);
  EXPECT_FALSE(tokens[1].opt.has_value());
}

TEST_F(CmdlineTest, clt_with_options) {
  auto tokens = clt.tokenize("cmd --hello=world");
  EXPECT_EQ(2, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("--hello", tokens[1].tok);
  EXPECT_TRUE(tokens[1].opt.has_value());
  EXPECT_EQ("world", tokens[1].opt.value());
}

TEST_F(CmdlineTest, clt_without_options) {
  auto tokens = clt.tokenize("cmd --hello");
  EXPECT_EQ(2, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("--hello", tokens[1].tok);
  EXPECT_FALSE(tokens[1].opt.has_value());
}

TEST_F(CmdlineTest, clt_short_with_options) {
  auto tokens = clt.tokenize("cmd -h world");
  EXPECT_EQ(2, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("-h", tokens[1].tok);
  EXPECT_TRUE(tokens[1].opt.has_value());
  EXPECT_EQ("world", tokens[1].opt.value());
}

TEST_F(CmdlineTest, clt_short) {
  auto tokens = clt.tokenize("cmd -h");
  EXPECT_EQ(2, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("-h", tokens[1].tok);
  EXPECT_FALSE(tokens[1].opt.has_value());
}

TEST_F(CmdlineTest, clt_short_with_quotes) {
  auto tokens = clt.tokenize("cmd -h \"hello world\"");
  EXPECT_EQ(2, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("-h", tokens[1].tok);
  EXPECT_TRUE(tokens[1].opt.has_value());
  EXPECT_EQ("hello world", tokens[1].opt.value());
}

TEST_F(CmdlineTest, clt_mixed_with_options) {
  auto tokens = clt.tokenize("cmd -h --hello=world");
  EXPECT_EQ(3, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("-h", tokens[1].tok);
  EXPECT_FALSE(tokens[1].opt.has_value());
  EXPECT_EQ("--hello", tokens[2].tok);
  EXPECT_TRUE(tokens[2].opt.has_value());
  EXPECT_EQ("world", tokens[2].opt.value());
}

TEST_F(CmdlineTest, clt_mixed_with_quotes) {
  auto tokens = clt.tokenize("cmd -h --hello=\"hello world\"");
  EXPECT_EQ(3, tokens.size());
  EXPECT_EQ("cmd", tokens[0].tok);
  EXPECT_FALSE(tokens[0].opt.has_value());
  EXPECT_EQ("-h", tokens[1].tok);
  EXPECT_FALSE(tokens[1].opt.has_value());
  EXPECT_EQ("--hello", tokens[2].tok);
  EXPECT_TRUE(tokens[2].opt.has_value());
  EXPECT_EQ("hello world", tokens[2].opt.value());
}

TEST_F(CmdlineTest, parse_options_1) {
  sled::cli::option_definition<std::string> arg1{"--arg1", "default"};

  auto [a] = sled::cli::parser::parse_cmd("cmd --arg1=new", arg1);
  EXPECT_EQ(a, "new");
}

TEST_F(CmdlineTest, parse_options_2) {
  sled::cli::option_definition<std::string> arg1{"--arg1", "default"};
  sled::cli::option_definition<std::string> arg2{"--arg2", "defaulted"};

  auto [a, b] = sled::cli::parser::parse_cmd("cmd --arg1=new", arg1, arg2);
  EXPECT_EQ(a, "new");
  EXPECT_EQ(b, "defaulted");
}

TEST_F(CmdlineTest, parse_options_2_ints) {
  sled::cli::option_definition<std::string> arg1{"--arg1", "default"};
  sled::cli::option_definition<int> arg2{"--arg2", 1};

  auto [a, b] = sled::cli::parser::parse_cmd("cmd --arg1=new", arg1, arg2);
  EXPECT_EQ(a, "new");
  EXPECT_EQ(b, 1);
}

TEST_F(CmdlineTest, parse_options_2_bool) {
  sled::cli::option_definition<std::string> arg1{"--arg1", "default"};
  sled::cli::option_definition<bool> arg2{"--arg2", false};

  auto [a, b] = sled::cli::parser::parse_cmd("cmd --arg1=new", arg1, arg2);
  EXPECT_EQ(a, "new");
  EXPECT_EQ(b, false);

  auto [a2, b2] =
      sled::cli::parser::parse_cmd("cmd --arg1=new --arg2", arg1, arg2);
  EXPECT_EQ(a2, "new");
  EXPECT_EQ(b2, true);
}

class MyCommand : public sled::cli::command {
 private:
  static sled::cli::option_definition<std::string> arg1_def;
  static sled::cli::option_definition<bool> arg2_def;

 public:
  MyCommand() : command(), arg1(arg1_def), arg2(arg2_def) {
    add_option(&arg1);
    add_option(&arg2);
  }

  sled::cli::option_inst<decltype(arg1_def)> arg1;
  sled::cli::option_inst<decltype(arg2_def)> arg2;
};

sled::cli::option_definition<std::string> MyCommand::arg1_def{"--arg1",
                                                              "default"};
sled::cli::option_definition<bool> MyCommand::arg2_def{"--arg2", false};

TEST_F(CmdlineTest, command_construct) {
  MyCommand cmd;

  cmd.parse_cmd("cmd --arg1=new --arg2");

  EXPECT_EQ("new", cmd.arg1.value_.value());
  EXPECT_TRUE(cmd.arg2.value_.value());
}

TEST_F(CmdlineTest, command_default) {
  MyCommand cmd;

  cmd.parse_cmd("cmd");

  EXPECT_EQ("default", cmd.arg1.value_or_default());
  EXPECT_FALSE(cmd.arg2.value_or_default());
}

TEST_F(CmdlineTest, getopt_test) {
  sled::cli::option_definition<std::string> opt_arg1{"--arg1", "default"};
  sled::cli::option_definition<bool> opt_arg2{"--arg2", false};

  auto result = sled::cli::parser::parse_cmd("cmd.exe --arg1=foo");
  EXPECT_EQ(1, std::tuple_size<decltype(result)>::value);
}
