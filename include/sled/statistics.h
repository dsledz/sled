/*
 * Copyright (c) 2018-2019, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <unordered_map>

#include "sled/platform.h"

namespace sled {

struct StatisticImpl {
  constexpr StatisticImpl() = default;

  // NOLINTNEXTLINE
  operator double() const { return static_cast<double>(v); }

  double v{0};
};

template <typename T>
struct Counter {
  constexpr Counter() = default;

  // NOLINTNEXTLINE
  constexpr Counter(T v) : v(v) {}

  // NOLINTNEXTLINE
  constexpr operator StatisticImpl() const {
    return StatisticImpl{static_cast<double>(v)};
  }

  a_forceinline constexpr Counter &operator+=(T rhs) {
    v += rhs;
    return *this;
  }

  a_forceinline constexpr const Counter<T> operator++(int) {
    v++;
    return *this;
  }

  T v{};
};

template <typename T>
struct Aggregate {
  constexpr Aggregate() = default;

  // NOLINTNEXTLINE
  constexpr operator StatisticImpl() const {
    return StatisticImpl{static_cast<double>(s) / c};
  }

  a_forceinline constexpr Aggregate &operator+=(T rhs) {
    s += rhs;
    c++;
    return *this;
  }

  T s;
  int c{};
};

class StatisticsReporter {
 public:
  virtual void add(const std::string &name, StatisticImpl stat) = 0;
  virtual StatisticImpl get(const std::string &name) = 0;
};

class StatisticsReport : public StatisticsReporter {
 public:
  void add(const std::string &name, StatisticImpl stat) override;
  StatisticImpl get(const std::string &name) override;

 private:
  std::unordered_map<std::string, StatisticImpl> m_statistics;
};

/**
 * All statistics trackers should inherit and implement report.
 */
struct StatsImpl {
  virtual void report_stats(StatisticsReporter &reporter) = 0;
};

}  // namespace sled
