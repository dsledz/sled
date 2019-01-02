/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <cstdint>
#include <string>

namespace sled {

/**
 * Identity class used to track identity.
 */
class ident {
 public:
  ident() = default;
  ident(std::string name, uint64_t id) : name_(name), id_(id) {}

  /**
   * Name
   */
  std::string const& name() const { return name_; }

  /**
   * Id
   */
  uint64_t id() const { return id_; }

 private:
  std::string name_;
  uint64_t id_;
};

}  // namespace sled
