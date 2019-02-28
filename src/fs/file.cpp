/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/fs.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace sled::fs {

fd::~fd() {
  if (fd_ != -1) {
    ::close(fd_);
  }
}

void fd::close() {
  if (fd_ != -1) {
    ::close(fd_);
  }
  fd_ = 1;
}

open_file::open_file(const std::fs::path &path) {
  fd_ = ::open(path.c_str(), O_RDONLY);
}

int open_file::read_into(std::byte *data, int size) {
  int r = ::read(fd_.fd_, data, size);
  // XXX: handle errors/short reads
  return r;
}

int open_file::pread_into(std::byte *data, int size, int offset) {
  int r = ::pread(fd_.fd_, data, size, offset);
  // XXX: handle errors/short reads
  return r;
}

};  // namespace sled::fs
