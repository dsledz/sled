/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include <string>
#include <iomanip>
#include <istream>
#include <map>
#include <ostream>

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

/**
 * Recursively concat types using the << operator.
 */
static a_forceinline void sstreamfn(std::ostream & /*os*/) {}
template <typename H, typename... T>
void sstreamfn(std::ostream &os, const H &p, T const &... t) {
  os << p;
  sstreamfn(os, t...);
}

} // namespace sled
