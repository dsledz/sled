/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <array>
#include "sled/platform.h"

namespace sled {

template <typename obj_type, int maximum>
class ring {
 public:
  constexpr ring() : m_objects() {}

  inline constexpr bool push_back(const obj_type& obj) {
    if (m_used == maximum) {
      return false;
    }
    m_objects[m_back] = obj;
    m_back = (m_back + 1) % maximum;
    m_used++;
    return true;
  }

  inline constexpr void pop_front() {
    m_front = (m_front + 1) % maximum;
    m_used--;
  }

  inline constexpr const obj_type& front() {
    debug_assert(m_used);
    return m_objects[m_front];
  }

  inline constexpr int size() { return m_used; }

  inline constexpr bool empty() { return m_used == 0; }

 private:
  int m_front{0};
  int m_back{0};
  int m_used{0};
  std::array<obj_type, maximum> m_objects;
};

}  // namespace sled
