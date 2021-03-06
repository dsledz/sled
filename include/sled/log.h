/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <array>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "sled/enum.h"
#include "sled/strong_int.h"

namespace sled::log {

/**
 * Log Severity.
 *
 * Higher severities are more important. Messages >= the default severity will
 * be logged.
 */
struct Severity : enum_struct<uint32_t, Severity> {
  static constexpr std::array<name_type, 9> names{
      std::make_pair(0, "Trace"),    std::make_pair(1, "Debug"),
      std::make_pair(2, "Info"),     std::make_pair(3, "Notice"),
      std::make_pair(4, "Warning"),  std::make_pair(5, "Error"),
      std::make_pair(6, "Critical"), std::make_pair(7, "Fatal"),
      std::make_pair(8, "Inherited")};

  using enum_struct::enum_struct;

  struct V;
};

struct Severity::V {
  static constexpr Severity Trace{0};
  static constexpr Severity Debug{1};
  static constexpr Severity Info{2};
  static constexpr Severity Notice{3};
  static constexpr Severity Warning{4};
  static constexpr Severity Error{5};
  static constexpr Severity Critical{6};
  static constexpr Severity Fatal{7};
  static constexpr Severity Inherited{8};
};

/**
 * Facility.
 *
 * TODO(dan): Figure out a way to support custom names.
 */
struct Facility : enum_struct<uint32_t, Facility> {
 public:
  static constexpr std::array<name_type, 8> names{
      std::make_pair(0, "None"),  std::make_pair(1, "Util"),
      std::make_pair(2, "Exec"),  std::make_pair(3, "Perf"),
      std::make_pair(4, "Test"),  std::make_pair(5, "User1"),
      std::make_pair(6, "User2"), std::make_pair(7, "User3")};

  using enum_struct<uint32_t, Facility>::enum_struct;

  struct V;
};

struct Facility::V {
  static constexpr Facility None{0};
  static constexpr Facility Util{1};
  static constexpr Facility Exec{2};
  static constexpr Facility Perf{3};
  static constexpr Facility Test{4};
  static constexpr Facility User1{5};
  static constexpr Facility User2{6};
  static constexpr Facility User3{7};
};

struct tsc_t : StrongInt<uint64_t, tsc_t> {
  using StrongInt<uint64_t, tsc_t>::StrongInt;

  static constexpr tsc_t now() { return tsc_t{0}; }
};

struct thr_id_t : StrongInt<uint64_t, thr_id_t> {
  using StrongInt<uint64_t, thr_id_t>::StrongInt;

  static thr_id_t self() { return thr_id_t{0}; }
};

struct task_id_t : StrongInt<uint64_t, task_id_t> {
  using StrongInt<uint64_t, task_id_t>::StrongInt;

  static task_id_t self() { return task_id_t{0}; }
};

/**
 * Log Message definition.
 *
 */
struct message {
 public:
  message(Facility facility, Severity severity, tsc_t tsc = tsc_t::now(),
          thr_id_t tid = thr_id_t::self(),
          task_id_t task_id = task_id_t::self())
      : facility(facility),
        severity(severity),
        tsc(tsc),
        thr_id(tid),
        task_id(task_id) {}

  /**
   * Format the message and return the stream buffer.
   *
   * This call is allowed to modify the internal state of
   * the message but should return the same effective result each time.
   */
  virtual std::streambuf *format() = 0;

  /**
   * Format and convert message to a string.
   */
  virtual std::string what() = 0;

 public:
  Facility facility;
  Severity severity;
  tsc_t tsc;
  thr_id_t thr_id;
  task_id_t task_id;
};

/**
 * Simple message that performs immediate construction to a string stream.
 */
struct stream_message final : public message {
 public:
  using message::message;

  std::ostream &msg() { return msg_; }
  std::streambuf *format() override { return msg_.rdbuf(); }
  std::string what() override { return msg_.str(); }

  void build() {}

  /**
   * Build a message using variable template expansion.
   */
  template <typename... Args, typename T>
  void build(T &&arg, Args... args) {
    msg_ << std::forward<T>(arg);
    build(args...);
  }

 private:
  std::stringstream msg_;
};

/**
 * Default log sink for an std::ostream.
 *
 * Additional sinks should have a similar signature.
 */
void sink_msg(std::ostream &sink, message &msg);

/**
 * A log sink.
 *
 * A log sink takes a message and saves it.
 */
class Sink {
 public:
  template <typename T, typename = std::enable_if_t<
                            sled::mp::not_<std::is_same<T, Sink>>::value>>
  Sink(T &x) : sink_(std::make_unique<model<T>>(x)) {}

  Sink(const Sink &x) : sink_(x.sink_->copy_()) {}
  Sink(Sink &&) noexcept = default;
  Sink &operator=(const Sink &x) {
    *this = Sink(x);
    return *this;
  }
  Sink &operator=(Sink &&) noexcept = default;

  friend void sink_msg(Sink &x, message &msg) { x.sink_->sink_msg_(msg); }

 private:
  struct Sinkable {
    virtual ~Sinkable() = default;
    virtual std::unique_ptr<Sinkable> copy_() const = 0;
    virtual void sink_msg_(message &msg) = 0;
  };
  template <typename T>
  struct model final : Sinkable {
    explicit model(T &x) : data_(x) {}
    std::unique_ptr<Sinkable> copy_() const override {
      return std::make_unique<model<T>>(*this);
    }
    void sink_msg_(message &msg) override { sink_msg(data_, msg); }
    T &data_;
  };
  std::unique_ptr<Sinkable> sink_;
};

/**
 * Logging Manager.
 *
 * The logging manager is a global singleton that manages the various sinks and
 * logging.
 */
class LoggingManager {
 public:
  explicit LoggingManager(Sink default_sink);
  ~LoggingManager();

  void set_threshold(Severity sev);
  void set_default_severity(Severity sev);
  void set_default_sink(Sink sink);

  void increase_threshold();
  void decrease_threshold();

  Severity threshold() { return threshold_sev_; }
  Facility add_facility(std::string const &facility);

  a_forceinline bool enabled(Severity sev) { return threshold_sev_ <= sev; }

  /*
   * TODO(dan): Add multiple sink support
   * add_sink(Sink sin)
   * remove_sink(Sink sink)
   */
  template <typename... Args>
  void log_always(Facility facility, Severity severity, Args... args) {
    stream_message msg(facility, severity);
    msg.build(args...);
    sink_msg(default_sink_, msg);
  }

 private:
  Severity threshold_sev_;
  Severity default_sev_;
  Sink default_sink_;
  std::atomic<uint32_t> last_facility_;
};
};  // namespace sled::log
