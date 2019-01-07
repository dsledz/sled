/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

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
struct Severity : StrongEnum<uint32_t, Severity> {
  static Severity const Trace;
  static Severity const Debug;
  static Severity const Informational;
  static Severity const Notice;
  static Severity const Warning;
  static Severity const Error;
  static Severity const Critical;
  static Severity const Fatal;

  static constexpr std::array<const char *, 8> names{
      "Trace",   "Debug", "Informational", "Notice",
      "Warning", "Error", "Critical",      "Fatal"};

 public:
  using StrongEnum<uint32_t, Severity>::StrongEnum;
};

constexpr Severity const Severity::Trace{0};
constexpr Severity const Severity::Debug{1};
constexpr Severity const Severity::Informational{2};
constexpr Severity const Severity::Notice{3};
constexpr Severity const Severity::Warning{4};
constexpr Severity const Severity::Error{5};
constexpr Severity const Severity::Critical{6};
constexpr Severity const Severity::Fatal{7};

/**
 * Facility.
 *
 * TODO(dan): Figure out a way to support custom names.
 */
struct Facility : StrongEnum<uint32_t, Facility> {
 public:
  static Facility const None;
  static Facility const Util;
  static Facility const Exec;
  static Facility const Perf;
  static Facility const Test;
  static Facility const User1;
  static Facility const User2;
  static Facility const User3;

  static constexpr std::array<const char *, 8> names{
      "None", "Util", "Exec", "Perf", "Test", "User1", "User2", "User3"};

 public:
  using StrongEnum<uint32_t, Facility>::StrongEnum;
};

constexpr Facility const Facility::None{0};
constexpr Facility const Facility::Util{1};
constexpr Facility const Facility::Exec{2};
constexpr Facility const Facility::Perf{3};
constexpr Facility const Facility::Test{4};
constexpr Facility const Facility::User1{5};
constexpr Facility const Facility::User2{6};
constexpr Facility const Facility::User3{7};

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
                            std::__not_<std::is_same<T, Sink>>::value>>
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

  void set_default_severity(Severity sev);
  void set_default_sink(Sink sink);

  Facility add_facility(std::string const &facility);

  a_forceinline bool enabled(Severity sev) { return default_sev_ <= sev; }

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
  Severity default_sev_;
  Sink default_sink_;
  std::atomic<uint32_t> last_facility_;
};
};  // namespace sled::log
