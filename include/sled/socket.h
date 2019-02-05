/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"
#include "sled/net_types.h"

namespace sled::net {
/**
 * tcp_socket
 */
class tcp_socket {
 public:
  tcp_socket();
  ~tcp_socket() = default;

  bool valid() { return fd_.valid(); };

  net_result bind(ip_port port) noexcept;
  net_result connect(ip_address address, ip_port port) noexcept;
  net_result listen(int backlog = 5) noexcept;

 private:
  net_fd fd_{-1};
};

class tcp_server {
  static constexpr int DEFAULT_BACKLOG = 5;
 public:
  tcp_server();
  ~tcp_server() = default;

  bool valid() { return sock_.valid(); }

  net_result listen(ip_port port) noexcept;

 private:
  tcp_socket sock_;
};

} // namespace sled::net
