/*
 * Copyright (c) 2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/socket.h"

#include "gtest/gtest.h"

class SocketTest : public ::testing::Test {
  protected:
   SocketTest() = default;

  static constexpr sled::net::ip_address default_ip = sled::net::ip_address(127,0,0,1);
  static constexpr sled::net::ip_port default_port = 1234;
  sled::net::tcp_socket sock;
};

TEST_F(SocketTest, valid) {
  EXPECT_TRUE(sock.valid());
}

TEST_F(SocketTest, connect_failure) {
  // TODO(dan):
  EXPECT_THROW(sock.connect(default_ip, default_port).result(), sled::net::net_exception);
}

TEST_F(SocketTest, bind_success) {
  EXPECT_EQ(0, sock.bind(default_port).result());
}

TEST_F(SocketTest, listen_success) {
  EXPECT_EQ(0, sock.bind(default_port).result());
  EXPECT_EQ(0, sock.listen(5).result());
}

#include "sled/epoll.h"

class EpollTest : public ::testing::Test {
  protected:
   EpollTest() = default;

   sled::net::epoll epoll;
};

TEST_F(EpollTest, create) {
  EXPECT_TRUE(epoll.valid());
}

TEST_F(EpollTest, one_shot) {
  EXPECT_TRUE(epoll.valid());
  sled::net::unix_pipe pipe;
}

