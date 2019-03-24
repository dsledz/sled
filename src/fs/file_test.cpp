/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/fs.h"

#include "gtest/gtest.h"

#include <iostream>

namespace sled::fs {

class FileTest : public ::testing::Test {
 protected:
  FileTest() = default;

  void SetUp() override { temp_dir_ = std::filesystem::temp_directory_path(); }

  void TearDown() override { std::cout << temp_dir_; }

  std::filesystem::path temp_dir_;
};

TEST_F(FileTest, openfile) {
  auto temp_file = temp_dir_ / "openfile";

  open_file tmp(temp_file);
}

}  // namespace sled::fs
