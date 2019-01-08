/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <algorithm>
#include <iomanip>
#include <istream>
#include <map>
#include <ostream>
#include <string>

namespace sled {

/**
 * Convert a string to lowercase using the current locale.
 */
static inline std::string str_tolower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

/**
 * Returns true iff @a s ends with @a e.
 */
static inline bool ends_with(const std::string &s, const std::string &e) {
  size_t n = s.rfind(e);
  if (n == std::string::npos) {
    return false;
  }
  size_t s_len = s.size();
  size_t e_len = e.size();
  if (s_len < e_len) {
    return false;
  }
  return (n == s_len - e_len);
}

template<typename T>
static inline std::string as_string(const std::string &hint, const T &obj) {
  std::stringstream ss;
  ss << hint << "<" << obj << ">";
  return ss.str();
}

// Recursive function to concat strings
template <typename H>
static a_forceinline std::string stringfn(const H &p) { return to_string(p); }
template <typename H, typename... T>
std::string stringfn(H const&p, T const &... t) {
  return to_string(p) + stringfn(t...);
}

/**
 * Recursively concat types using the << operator.
 */
static a_forceinline void sstreamfn(std::ostream & /*os*/) {}
template <typename H, typename... T>
void sstreamfn(std::ostream &os, H const &p, T const &... t) {
  os << p;
  sled::sstreamfn(os, t...);
}

} // namespace sled
