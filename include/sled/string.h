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

#include "sled/platform.h"

namespace sled {

/**
 * Convert a string to lowercase using the current locale.
 */
static inline void str_tolower(std::string &s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

/**
 * Convert a string to lowercase using the current locale.
 * Return a copy.
 */
static inline std::string str_tolower_copy(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

/**
 * string insentive equals
 */
static inline bool str_iequal(const std::string &lhs, const std::string &rhs) {
  return std::equal(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
      [](char a, char b) { return ::tolower(a) == ::tolower(b); });
}

/**
 * Returns true iff @a s ends with @a e.
 */
static inline bool ends_with(const std::string &haystack,
                             const std::string &needle) {
  size_t n = haystack.rfind(needle);
  if (n == std::string::npos) {
    return false;
  }
  size_t s_len = haystack.size();
  size_t e_len = needle.size();
  if (s_len < e_len) {
    return false;
  }
  return (n == s_len - e_len);
}

/**
 * Return a formated obj as a string.
 */
template <typename T>
static inline std::string as_string(const std::string &prefix, const T &obj) {
  std::stringstream ss;
  ss << prefix << "<" << obj << ">";
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

// From:
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

} // namespace sled
