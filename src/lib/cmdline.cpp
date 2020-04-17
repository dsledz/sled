/*
 * Copyright (c) 2020, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/cmdline.h"

#include <sstream>

namespace sled::cli {

std::vector<token> tokenizer::tokenize(const std::string cmdline) {
  std::vector<token> tokens;
  std::stringstream ss(cmdline);
  while (!ss.bad() && !ss.eof()) {
    auto tok_opt = next_token(ss);
    if (!tok_opt.has_value()) {
      return tokens;
    }
    tokens.push_back(std::move(tok_opt.value()));
  }
  return tokens;
}

std::vector<token> tokenizer::tokenize(int argc, const char *const *argv) {
  std::vector<token> tokens;
  for (int i = 0; i < argc; i++) {
    auto &last = tokens.emplace_back(token{argv[i]});
    if (sled::begins_with(last.tok, "-")) {
      std::string arg;
      size_t found;
      // Split if we find a '='
      // XXX: Handle quotes
      if ((found = last.tok.find("=")) != std::string::npos) {
        last.opt = last.tok.substr(found + 1);
        last.tok = last.tok.substr(0, found);
      } else if (i + 1 < argc && !sled::begins_with(argv[i + 1], "-")) {
        last.opt = argv[i + 1];
        i++;
      }
    }
  }
  return tokens;
}

std::optional<token> tokenizer::next_token(std::istream &is) {
  std::string tmp = "";
  std::string arg = "";
  char c;
  bool have_dash = false;
  bool have_arg = false;
  bool have_quote = false;
  for (is >> std::noskipws >> c; !is.bad() && !is.eof();
       is >> std::noskipws >> c) {
    switch (c) {
      case '-': {
        if (have_quote) {
          tmp += c;
        } else if (!have_dash && !have_arg && tmp.empty()) {
          // Start of arg
          have_dash = true;
          tmp += c;
        } else if (have_arg) {
          // Found the start of another option, bail out
          is.putback(c);
          return token{arg};
        } else {
          // dash inside arg or option
          tmp += c;
        }
        break;
      }
      case '=': {
        if (have_quote) {
          tmp += c;
        } else {
          arg = tmp;
          have_arg = true;
          tmp = "";
        }
        break;
      }
      case '"': {
        if (have_quote) {
          return token{arg, tmp};
        }
        if (have_arg) {
          have_quote = true;
        }
        break;
      }
      case ' ': {
        if (have_quote) {
          tmp += c;
        } else if (!have_arg) {
          if (!have_dash) {
            return token{tmp};
          }
          // Start of option
          arg = tmp;
          have_arg = true;
          tmp = "";
        } else if (!tmp.empty()) {
          // End of '--arg=option'
          return token{arg, tmp};
        } else {
          // Spaces are skipped outside of quotes
        }
        break;
      }
      default: {
        tmp += c;
        break;
      }
    }
  }
  // Return what we have
  // XXX: Is this correct?
  if (have_arg) {
    return token{arg, tmp};
  } else if (!tmp.empty()) {
    return token{tmp};
  } else {
    return std::nullopt;
  }
}

}  //  namespace sled::cli
