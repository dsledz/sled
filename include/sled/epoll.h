/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"
#include "sled/net_types.h"
#include "sled/socket.h"
#include "sled/executor.h"

namespace sled::net {

class unix_pipe {
  public:
   unix_pipe();

   net_result create();

   net_fd &fd1() { return fd1_; }
   net_fd &fd2() { return fd2_; }

  private:

   net_fd fd1_;
   net_fd fd2_;
};

/**
 * epoll executor.
 */
class epoll {
 public:
  epoll();
  ~epoll() = default;

  bool valid() noexcept { return fd_.valid(); }

  net_result one_shot(net_fd fd) noexcept;

private:
  net_fd fd_{};
};

} // namespace sled::net
