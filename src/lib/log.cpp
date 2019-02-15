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
    : threshold_sev_(Severity::Notice),
      default_sev_(Severity::Notice),
      default_sink_(default_sink),
      last_facility_(Facility::User3.v) {}

LoggingManager::~LoggingManager() = default;

void LoggingManager::set_threshold(Severity sev) { threshold_sev_ = sev; }

void LoggingManager::set_default_severity(Severity sev) { default_sev_ = sev; }

void LoggingManager::set_default_sink(Sink sink) { default_sink_ = sink; }

void LoggingManager::increase_threshold() {
  switch (threshold_sev_) {
    case sled::log::Severity::Inherited:
      break;
    case sled::log::Severity::Trace:
      break;
    case sled::log::Severity::Debug:
      threshold_sev_ = sled::log::Severity::Trace;
      break;
    case sled::log::Severity::Info:
      threshold_sev_ = sled::log::Severity::Debug;
      break;
    case sled::log::Severity::Notice:
      threshold_sev_ = sled::log::Severity::Info;
      break;
    case sled::log::Severity::Warning:
      threshold_sev_ = sled::log::Severity::Notice;
      break;
    case sled::log::Severity::Error:
      threshold_sev_ = sled::log::Severity::Warning;
      break;
    case sled::log::Severity::Critical:
      threshold_sev_ = sled::log::Severity::Error;
      break;
    case sled::log::Severity::Fatal:
      threshold_sev_ = sled::log::Severity::Critical;
      break;
  }
}

void LoggingManager::decrease_threshold() {
  switch (threshold_sev_) {
    case sled::log::Severity::Inherited:
      break;
    case sled::log::Severity::Trace:
      threshold_sev_ = sled::log::Severity::Debug;
      break;
    case sled::log::Severity::Debug:
      threshold_sev_ = sled::log::Severity::Info;
      break;
    case sled::log::Severity::Info:
      threshold_sev_ = sled::log::Severity::Notice;
      break;
    case sled::log::Severity::Notice:
      threshold_sev_ = sled::log::Severity::Warning;
      break;
    case sled::log::Severity::Warning:
      threshold_sev_ = sled::log::Severity::Error;
      break;
    case sled::log::Severity::Error:
      threshold_sev_ = sled::log::Severity::Critical;
      break;
    case sled::log::Severity::Critical:
      threshold_sev_ = sled::log::Severity::Fatal;
      break;
    case sled::log::Severity::Fatal:
      break;
  }
}

Facility LoggingManager::add_facility(std::string const &facility) {
  return Facility{++last_facility_};
}
}  // namespace sled::log
