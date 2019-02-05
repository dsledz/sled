/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#include "sled/socket.h"

#include "errno.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "fcntl.h"
#include "unistd.h"
#include "arpa/inet.h"

namespace sled::net {

net_result::net_result(int result) noexcept : result_(result) {
  if (result_ == -1) {
    error_ = net_error(errno);
  }
}

int net_result::result() {
  if (error_) {
    throw net_exception(error_);
  }
  return result_;
}

net_fd::~net_fd() {
  if (fd_ != -1) {
    ::close(fd_);
  }
}

uint32_t ip_address::nbo() noexcept {
  return htonl(v);
}

uint16_t ip_port::nbo() noexcept {
  return htons(v);
}

tcp_socket::tcp_socket() : fd_(net_result(::socket(AF_INET, SOCK_STREAM, 0))) {}

net_result tcp_socket::bind(ip_port port) noexcept {
  sockaddr_in sa;

  sa.sin_family = AF_INET;
  sa.sin_port = port.nbo();
  sa.sin_addr.s_addr = INADDR_ANY;

  net_result result;

  int one = 1;
  result = ::setsockopt(fd_.value(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  if (result.error()) {
    return result;
  }

  result = ::bind(fd_.value(), (struct sockaddr *)&sa, sizeof(sa));

  return result;
}

net_result tcp_socket::connect(ip_address address, ip_port port) noexcept {
  sockaddr_in sa;

  sa.sin_family = AF_INET;
  sa.sin_port = port.nbo();
  sa.sin_addr.s_addr = address.nbo();

  net_result result(::connect(fd_.value(), (struct sockaddr *)&sa, sizeof(sa)));

  return result;

}

net_result tcp_socket::listen(int backlog) noexcept {
  net_result result;

  result = net_result(::listen(fd_.value(), backlog));

  return result;
}

tcp_server::tcp_server() = default;

net_result tcp_server::listen(ip_port port) noexcept {
  net_result result;

  result = sock_.bind(port);
  if (result.error()) {
    return result;
  }

  result = sock_.listen(DEFAULT_BACKLOG);

  return result;
}

} //namespace sled::net
