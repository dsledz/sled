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
    : default_sev_(Severity::Notice),
      default_sink_(default_sink),
      last_facility_(Facility::User3.v) {}

LoggingManager::~LoggingManager() = default;

void LoggingManager::set_default_severity(Severity sev) { default_sev_ = sev; }

void LoggingManager::set_default_sink(Sink sink) { default_sink_ = sink; }

Facility LoggingManager::add_facility(std::string const &facility) {
  return Facility{++last_facility_};
}
}  // namespace sled::log
