/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/epoll.h"

#include "errno.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "fcntl.h"
#include "unistd.h"
#include "arpa/inet.h"

#include "sys/epoll.h"

namespace sled::net {

unix_pipe::unix_pipe() {
  int fds[2];

  // TODO(dan): This interface throws
  net_result result(::pipe2(fds, O_NONBLOCK));
  result.result();

  fd1_ = net_fd(fds[0]);
  fd2_ = net_fd(fds[1]);
}

epoll::epoll() : fd_(net_result(::epoll_create1(EPOLL_CLOEXEC)).result()) {}

net_result epoll::one_shot(net_fd fd) noexcept {
  net_result result;

  return result;
}

}
