/*
 * Copyright (c) 2018, Dan Sledz
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/platform.h"
#include "sled/strong_int.h"

namespace sled {

struct nsec final : public StrongInt<int64_t, nsec> {
  using StrongInt<int64_t, nsec>::StrongInt;

  static inline constexpr nsec from_ts(struct timespec const &t) {
    return nsec{t.tv_sec * 1'000'000'000ll + t.tv_nsec};
  }

  static constexpr auto NSECS = 1ll;
};

struct usec final : public StrongInt<int64_t, nsec> {
  using StrongInt<int64_t, nsec>::StrongInt;
  static constexpr auto NSECS = 1'000ll;
};

struct msec final : public StrongInt<int64_t, nsec> {
  using StrongInt<int64_t, nsec>::StrongInt;
  static constexpr auto NSECS = 1'000'000ll;
};

struct sec final : public StrongInt<int64_t, nsec> {
  using StrongInt<int64_t, nsec>::StrongInt;
  static constexpr auto NSECS = 1'000'000'000ll;
};

struct time final : public StrongInt<int64_t, time> {
  // NOLINTNEXTLINE
  constexpr time() = default;
  constexpr time(int64_t t) : StrongInt<int64_t, time>(t) {}
  template <typename T>
  constexpr time(T t) : StrongInt<int64_t, time>(t.v * T::NSECS) {}
  // NOLINTNEXTLINE
  inline explicit constexpr operator double() const {
    return double(v) / sec::NSECS;
  }
  // NOLINTNEXTLINE
  inline constexpr operator msec() const { return msec(v / msec::NSECS); }
  // NOLINTNEXTLINE
  inline constexpr operator usec() const { return usec(v / usec::NSECS); }
  // NOLINTNEXTLINE
  inline constexpr operator nsec() const { return nsec(v / nsec::NSECS); }
  // NOLINTNEXTLINE
  inline constexpr operator sec() const { return sec(v / sec::NSECS); }

  static inline constexpr time from_nsec(int64_t v) { return time(nsec(v)); }

  static inline constexpr time from_usec(int64_t v) { return time(usec(v)); }

  static inline constexpr time from_msec(int64_t v) { return time(msec(v)); }

  static inline constexpr time from_sec(int64_t v) { return time(sec(v)); }

  constexpr size_t reciprocal(size_t num) const noexcept {
    if (v == 0) {
      return 0;
    }
    num *= sec::NSECS;
    return num / v;
  }

  constexpr double reciprocal_double(size_t num) const noexcept {
    if (v == 0) {
      return 0;
    }
    double tmp = static_cast<double>(num) * sec::NSECS;
    return tmp / v;
  }

  friend std::ostream &operator<<(std::ostream &os, time const &obj) {
    int64_t ns = obj.v;
    if (ns < 0) {
      os << "-";
      ns = -ns;
    }
    if (ns < usec::NSECS) {
      os << ns << "ns";
    } else if (ns < msec::NSECS) {
      os << ns / usec::NSECS;
      int64_t remainder = ns % usec::NSECS;
      if (remainder) {
        os << "." << remainder / (usec::NSECS / 1000);
      }
      os << "us";
    } else if (ns < sec::NSECS) {
      os << ns / msec::NSECS;
      int64_t remainder = ns % msec::NSECS;
      if (remainder) {
        os << "." << remainder / (msec::NSECS / 1000);
      }
      os << "ms";
    } else {
      os << ns / sec::NSECS;
      int64_t remainder = ns % sec::NSECS;
      if (remainder) {
        os << "." << remainder / (sec::NSECS / 1000);
      }
      os << "s";
    }
    return os;
  }

  friend inline std::string to_string(time const &obj) {
    std::stringstream ss;
    ss << obj;
    return ss.str();
  }

  constexpr inline time operator/(int rhs) noexcept { return time{v / rhs}; }

};

static constexpr time time_zero{nsec{0}};

#ifndef WIN32
static inline void timespec_diff(struct timespec *dest, struct timespec first,
                                 struct timespec second) {
  if (second.tv_nsec < first.tv_nsec) {
    second.tv_sec -= 1;
    second.tv_nsec += sec::NSECS;
  }
  dest->tv_sec = first.tv_sec - second.tv_sec;
  dest->tv_nsec = first.tv_nsec - second.tv_nsec;
}

/**
 * Implementation of a stopwatch/clock based on a high precision counter.
 */
class stopwatch {
 public:
  stopwatch() {
    clock_gettime(CLOCK_MONOTONIC, &start_);
    clock_ = start_;
  }
  ~stopwatch() = default;

  void reset() {
    clock_gettime(CLOCK_MONOTONIC, &start_);
    clock_ = start_;
  }

  /**
   * TODO(dan): pause/resume are unlikely to work.
   */
  void pause() { clock_gettime(CLOCK_MONOTONIC, &pause_); }

  void resume() {
    struct timespec now {};
    clock_gettime(CLOCK_MONOTONIC, &now);
    timespec_diff(&start_, now, pause_);
    pause_.tv_sec = 0;
    pause_.tv_nsec = 0;
  }

  /**
   * Return the current time without taking a split.
   */
  time current() {
    struct timespec now {};
    struct timespec runtime {};
    clock_gettime(CLOCK_MONOTONIC, &now);
    timespec_diff(&runtime, now, start_);

    return {nsec::from_ts(runtime)};
  }

  /**
   * Trigger a split.  The time from the last split is recorded.
   */
  time split() {
    struct timespec now {};
    struct timespec split {};
    clock_gettime(CLOCK_MONOTONIC, &now);
    timespec_diff(&split, now, clock_);
    clock_ = now;

    return {nsec::from_ts(split)};
  }

  /**
   * Return the current absolute time.
   *
   * This time is monotonically increasing but is not based on application
   * start time.
   */
  static time now() {
    struct timespec now {};
    clock_gettime(CLOCK_MONOTONIC, &now);
    return {nsec::from_ts(now)};
  }

 private:
  struct timespec clock_ {};
  struct timespec start_ {};
  struct timespec pause_ {};
};

#else

class stopwatch {
 public:
  stopwatch() {
    QueryPerformanceFrequency(&frequency_);
    reset();
  }
  ~stopwatch() = default;
  void reset() {
    QueryPerformanceCounter(&start_);
    clock_ = start_;
  }
  void pause() { QueryPerformanceCounter(&pause_); }
  void resume() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    start_.QuadPart = now.QuadPart - pause_.QuadPart;
    pause_.QuadPart = 0;
  }
  time current() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    LARGE_INTEGER runtime;
    runtime.QuadPart = now.QuadPart - start_.QuadPart;

    return {nsec{runtime.QuadPart}};
  }
  time split() {
    LARGE_INTEGER now;
    LARGE_INTEGER split;
    QueryPerformanceCounter(&now);
    split.QuadPart = now.QuadPart - clock_.QuadPart;
    clock_ = now;

    return {nsec{split.QuadPart}};
  }
  static time now() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return {nsec{now.QuadPart}};
  }

 private:
  LARGE_INTEGER clock_{};
  LARGE_INTEGER start_{};
  LARGE_INTEGER pause_{};
  LARGE_INTEGER frequency_{};
};
#endif

}  // namespace sled
