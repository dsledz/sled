/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/log.h"

namespace sled::log {

void sink_msg(std::ostream &sink, message &msg) {
  sink << msg.format();
  sink << std::endl;
  sink.flush();
}

LoggingManager::LoggingManager(Sink default_sink)
    : threshold_sev_(Severity::V::Notice),
      default_sev_(Severity::V::Notice),
      default_sink_(default_sink),
      last_facility_(Facility::V::User3.v) {}

LoggingManager::~LoggingManager() = default;

void LoggingManager::set_threshold(Severity sev) { threshold_sev_ = sev; }

void LoggingManager::set_default_severity(Severity sev) { default_sev_ = sev; }

void LoggingManager::set_default_sink(Sink sink) { default_sink_ = sink; }

void LoggingManager::increase_threshold() {
  switch (threshold_sev_) {
    case sled::log::Severity::V::Inherited:
    case sled::log::Severity::V::Trace:
      break;
    case sled::log::Severity::V::Debug:
      threshold_sev_ = sled::log::Severity::V::Trace;
      break;
    case sled::log::Severity::V::Info:
      threshold_sev_ = sled::log::Severity::V::Debug;
      break;
    case sled::log::Severity::V::Notice:
      threshold_sev_ = sled::log::Severity::V::Info;
      break;
    case sled::log::Severity::V::Warning:
      threshold_sev_ = sled::log::Severity::V::Notice;
      break;
    case sled::log::Severity::V::Error:
      threshold_sev_ = sled::log::Severity::V::Warning;
      break;
    case sled::log::Severity::V::Critical:
      threshold_sev_ = sled::log::Severity::V::Error;
      break;
    case sled::log::Severity::V::Fatal:
      threshold_sev_ = sled::log::Severity::V::Critical;
      break;
  }
}

void LoggingManager::decrease_threshold() {
  switch (threshold_sev_) {
    case sled::log::Severity::V::Inherited:
      break;
    case sled::log::Severity::V::Trace:
      threshold_sev_ = sled::log::Severity::V::Debug;
      break;
    case sled::log::Severity::V::Debug:
      threshold_sev_ = sled::log::Severity::V::Info;
      break;
    case sled::log::Severity::V::Info:
      threshold_sev_ = sled::log::Severity::V::Notice;
      break;
    case sled::log::Severity::V::Notice:
      threshold_sev_ = sled::log::Severity::V::Warning;
      break;
    case sled::log::Severity::V::Warning:
      threshold_sev_ = sled::log::Severity::V::Error;
      break;
    case sled::log::Severity::V::Error:
      threshold_sev_ = sled::log::Severity::V::Critical;
      break;
    case sled::log::Severity::V::Critical:
      threshold_sev_ = sled::log::Severity::V::Fatal;
      break;
    case sled::log::Severity::V::Fatal:
      break;
  }
}

Facility LoggingManager::add_facility(std::string const &facility) {
  return Facility{++last_facility_};
}
}  // namespace sled::log
