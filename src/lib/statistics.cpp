/*
 * Copyright (c) 2018-2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/statistics.h"

namespace sled {

void StatisticsReport::add(const std::string &name, StatisticImpl stat) {
  // XXX: Handle overwrites
  m_statistics[name] = stat;
}

StatisticImpl StatisticsReport::get(const std::string &name) {
  auto it = m_statistics.find(name);
  if (it == m_statistics.end()) {
    return StatisticImpl{};
  }
  return it->second;
}

}  // namespace sled
