/*
 * Copyright (c) 2020, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <histedit.h>
#include <initializer_list>
#include <type_traits>
#include "sled/enum.h"
#include "sled/platform.h"
#include "sled/string.h"
#include "string.h"

namespace sled::cli {

/**
 * Command line token.
 *
 * Each token is composed of either a single string (tok) for normal words.
 * For tokens starting in -, an optional second string is either taken by
 * splitting on '=' or ' ' and honors quotes '"' for embedded spaces.
 *
 * The strings are trimmed but otherwise not modified/inspected.
 */
struct token {
  token(std::string tok, std::string opt) : tok(tok), opt(opt) {}
  token(std::string tok) : tok(tok), opt(std::nullopt) {}

  std::string tok;
  std::optional<std::string> opt;
};

/**
 * Tokenizer
 *
 * Generates a vector of tokens from a getopt like string.
 *
 * Examples:
 * cmd --arg=opt
 * cmd --arg
 * cmd -a blah
 * cmd object verb --arg=opt
 */
class tokenizer {
 public:
  tokenizer() = default;

  /**
   * Tokenize an entire string.
   */
  std::vector<token> tokenize(const std::string cmdline);

  /**
   * Tokenize a main style argc/argv
   */
  std::vector<token> tokenize(int argc, const char *const *argv);

  /**
   * Return the next token in the stream
   */
  std::optional<token> next_token(std::istream &is);
};

/**
 * Option flag enumeration
 */
struct option_flag final : sled::enum_struct<uint32_t, option_flag> {
  static constexpr std::array<name_type, 1> names{
      std::make_pair(0x01, "required"),
  };

  using enum_struct::enum_struct;

  struct V;
};

struct option_flag::V {
  static constexpr option_flag required{0x01};
};

/**
 * Options flags
 *
 * The behavior of each option instance can be modified by one or more flags,
 * the most common one being "required"
 */
struct option_flags : public sled::flags_struct<option_flag, option_flags> {
  using flags_struct::flags_struct;
};

/**
 * Option Definition.
 *
 * A static description of an option.  The expectation is that a program with
 * multiple commands will re-use option names for the same semantic purpose.
 *
 * The information in the option_definition structure is used to construct the
 * help message.
 */
template <typename T>
struct option_definition {
  using opt_type = T;

  option_definition(std::string name, T default_value)
      : name(std::move(name)), default_value(std::move(default_value)) {}

  std::string name;
  opt_type default_value;
};

/**
 * Option.
 *
 * An instance of an option, used by a command to parse and store a user
 * supplied option.
 * Input must be sanitized before use.
 * value_ is unset if the option was not provided.
 */
template <typename Tdef>
struct option_inst {
  using opt_type = typename Tdef::opt_type;

  option_inst(Tdef definition, option_flags flags = {})
      : def_(definition), flags_(flags) {}

  friend void parse_token(option_inst &inst, const token &tok) {
    if (!tok.opt.has_value()) {
      if constexpr (std::is_same<opt_type, bool>()) {
        inst.value_ = true;
      } else {
        inst.value_ = std::nullopt;
      }
    } else if constexpr (std::is_same<opt_type, std::string>()) {
      inst.value_ = tok.opt.value();
    } else if constexpr (std::is_integral<opt_type>()) {
      inst.value_ = std::stoi(tok.opt.value());
    } else if constexpr (std::is_same<opt_type, bool>()) {
      auto tmp = sled::str_tolower_copy(tok.opt.value());
      if (tmp == "true" || tmp == "y" || tmp == "yes") {
        inst.value_ = true;
      } else {
        inst.value_ = false;
      }
    } else {
      inst.value_ = opt_type::from_string(tok.opt.value());
    }
  }

  opt_type &value_or_default() {
    if (value_.has_value()) {
      return value_.value();
    } else {
      return def_.default_value;
    }
  }

  friend std::string option_name(option_inst &inst) { return inst.def_.name; }

  Tdef def_;
  option_flags flags_;
  std::optional<opt_type> value_;
};

/**
 * Option Object concept.
 *
 * Objects supporting this concept should implement:
 * void parse_token(obj &x, const sled::cli::token &tok);
 */
class option_obj {
 public:
  template <typename T>
  option_obj(T *x) : self_(std::make_unique<model<T> >(std::move(x))) {}

  option_obj(const option_obj &x) : self_(x.self_->copy_()) {}
  option_obj(option_obj &&) noexcept = default;
  option_obj &operator=(const option_obj &x) {
    *this = option_obj(x);
    return *this;
  }
  option_obj &operator=(option_obj &&) noexcept = default;
  ~option_obj() = default;

  friend void parse_token(const option_obj &x, const token &tok) {
    return x.self_->parse_token_(tok);
  }
  friend std::string option_name(const option_obj &x) {
    return x.self_->option_name_();
  }

 private:
  struct concept_t {
    concept_t() = default;
    concept_t(concept_t &&rhs) = default;
    concept_t(const concept_t &rhs) = default;
    concept_t &operator=(concept_t &&rhs) = default;
    concept_t &operator=(const concept_t &rhs) = default;
    virtual ~concept_t() = default;
    virtual std::unique_ptr<concept_t> copy_() const = 0;
    virtual void parse_token_(const token &tok) = 0;
    virtual std::string option_name_() = 0;
  };
  template <typename T>
  struct model final : concept_t {
    explicit model(T *x) : data_(std::move(x)) {}
    std::unique_ptr<concept_t> copy_() const override {
      return std::make_unique<model>(*this);
    }
    void parse_token_(const token &tok) final { parse_token(*data_, tok); }
    std::string option_name_() final { return option_name(*data_); }
    T *data_;
  };
  std::unique_ptr<concept_t> self_;
};

/**
 * Simple command line parser.
 *
 * One-shot command line parser.
 * Usage:
 * option_definition<std::string> def_a("--arg1", "default");
 * ...
 * auto [a, b, c] = parser::parse_cmd(cmd_line_str, def_a, def_b, def_c);
 */
class parser {
 public:
  parser() = default;

  template <typename... Targ>
  static auto parse_cmd(const std::string &cmd_line, Targ const &... targs) {
    tokenizer clt;
    auto tokens = clt.tokenize(cmd_line);

    return parse_helper(tokens, targs...);
  }

 private:
  static auto parse_helper(std::vector<token> &tokens) {
    return std::make_tuple();
  }

  template <typename T, typename... Targ>
  static auto parse_helper(std::vector<token> &tokens,
                           const option_definition<T> &opt) {
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      if (sled::begins_with(opt.name, it->tok)) {
        auto val = T::from_string(it->opt.value());
        return std::make_tuple(val);
      }
    }
    return std::make_tuple(opt.default_value);
  }

  template <typename T, typename... Targ>
  static auto parse_helper(std::vector<token> &tokens,
                           const option_definition<T> &opt,
                           Targ const &... targs) {
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      if (sled::begins_with(opt.name, it->tok)) {
        auto val = T::from_string(it->opt.value());
        return std::tuple_cat(std::make_tuple(val),
                              parse_helper(tokens, targs...));
      }
    }
    return std::tuple_cat(std::make_tuple(opt.default_value),
                          parse_helper(tokens, targs...));
  }

  template <typename... Targ>
  static auto parse_helper(std::vector<token> &tokens,
                           const option_definition<std::string> &opt,
                           Targ const &... targs) {
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      if (sled::begins_with(opt.name, it->tok)) {
        auto val = it->opt.value();
        return std::tuple_cat(std::make_tuple(val),
                              parse_helper(tokens, targs...));
      }
    }
    return std::tuple_cat(std::make_tuple(opt.default_value),
                          parse_helper(tokens, targs...));
  }

  template <typename... Targ>
  static auto parse_helper(std::vector<token> &tokens,
                           const option_definition<int> &opt,
                           Targ const &... targs) {
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      if (sled::begins_with(opt.name, it->tok)) {
        auto val = std::stoi(it->opt.value().c_str(), nullptr, 0);
        return std::tuple_cat(std::make_tuple(val),
                              parse_helper(tokens, targs...));
      }
    }
    return std::tuple_cat(std::make_tuple(opt.default_value),
                          parse_helper(tokens, targs...));
  }

  template <typename... Targ>
  static auto parse_helper(std::vector<token> &tokens,
                           const option_definition<bool> &opt,
                           Targ const &... targs) {
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      if (sled::begins_with(opt.name, it->tok)) {
        auto val = [&]() {
          if (it->opt.has_value()) {
            auto tmp = sled::str_tolower_copy(it->opt.value());
            if (tmp == "true" || tmp == "y" || tmp == "yes") {
              return true;
            } else {
              return false;
            }
          } else {
            return true;
          }
        }();
        return std::tuple_cat(std::make_tuple(val),
                              parse_helper(tokens, targs...));
      }
    }
    return std::tuple_cat(std::make_tuple(opt.default_value),
                          parse_helper(tokens, targs...));
  }
};

/**
 * CLI Command.
 *
 * Abstract representation of a command with arguments.
 */
class command {
 public:
  command() {}

  template <typename T>
  void add_option(T *inst) {
    options_.emplace_back(std::move(inst));
  }

  void parse_cmd(const std::string &cmd_line) {
    tokenizer clt;
    auto tokens = clt.tokenize(cmd_line);

    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      auto opt = find_if(options_.begin(), options_.end(), [&](auto &tmp) {
        return sled::begins_with(option_name(tmp), it->tok);
      });
      if (opt != options_.end()) {
        parse_token(*opt, *it);
      }
    }
  }

  void parse_cmd(int argc, const char *const *argv) {
    tokenizer clt;
    auto tokens = clt.tokenize(argc, argv);

    for (auto it = tokens.begin(); it != tokens.end(); it++) {
      auto opt = find_if(options_.begin(), options_.end(), [&](auto &tmp) {
        return sled::begins_with(option_name(tmp), it->tok);
      });
      if (opt != options_.end()) {
        parse_token(*opt, *it);
      }
    }
  }

 private:
  std::vector<option_obj> options_;
};

}  //  namespace sled::cli
