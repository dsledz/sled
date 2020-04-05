/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include "sled/exception.h"
#include "sled/numeric.h"

namespace sled {

template <typename Ta, typename Tb, typename Tag>
struct struct_bitfield {
  static_assert(std::is_integral_v<Ta>);
  static_assert(sizeof(Ta) == sizeof(Tb));

  union {
    Ta v;
    Tb b;
  };

  struct_bitfield() { memset(this, 0, sizeof(*this)); }

  struct_bitfield(Ta const &v) : v(v) {}

  struct_bitfield(Tb const &v) : b(b) {}

  struct field_description {
    constexpr field_description(int l, int h, char const *label)
        : l(l), h(h), label(label) {}

    int l;
    int h;
    const char *label;
  };

  static constexpr field_description make_field(int l, int h,
                                                char const *label) {
    return field_description(l, h, label);
  }

  Tag &operator=(Ta const &rhs) {
    v = rhs;
    return *this;
  }

  friend inline std::ostream &operator<<(std::ostream &os, Tag const &rhs) {
    Ta val = rhs.v;
    bool first = true;
    for (auto &field : Tag::fields) {
      Ta p = (~((~0) << (field.h + 1)) & val) >> field.l;
      if (first) {
        first = false;
      } else {
        os << ", ";
      }
      HexFmt h(p, {HexFormat::NoPrefix, HexFormat::NoPadding});
      h.w = ((field.h - field.l) + 4) / 4;
      os << field.label << ":" << h;
    }
    return os;
  }

  friend inline std::string fmt_string(Tag const &rhs) {
    std::stringstream ss;
    ss << rhs;
    return ss.str();
  }

  static inline Tag from_string(std::string const &obj) {
    return Tag(stoull(obj, nullptr, 0));
  }
};

template <typename Ta, typename Tb, typename Tag>
struct flags_bitfield {
  static_assert(std::is_integral_v<Ta>);
  static_assert(sizeof(Ta) == sizeof(Tb));

  union {
    Ta v;
    Tb b;
  };

  flags_bitfield() { memset(this, 0, sizeof(*this)); }

  flags_bitfield(Ta const &v) : v(v) {}

  flags_bitfield(Tb const &v) : b(b) {}

  struct field_description {
    constexpr field_description(int b, char const *label)
        : b(b), label(label) {}

    int b;
    const char *label;
  };

  static constexpr field_description make_field(int b, char const *label) {
    return field_description(b, label);
  }

  Tag &operator=(Ta const &rhs) {
    v = rhs;
    return *this;
  }

  static inline Tag from_string(std::string const &obj) {
    return Tag(stoull(obj, nullptr, 0));
  }

  struct flags_bitfield_fmt {
    explicit constexpr flags_bitfield_fmt(const Tag &f) : f_(f) {}
    const Tag &f_;

    friend inline std::ostream &operator<<(std::ostream &os,
                                           const flags_bitfield_fmt &obj) {
      Ta val = obj.f_.v;
      bool first = true;
      for (auto &field : Tag::fields) {
        if ((val & (1 << field.b)) == 0) {
          continue;
        }
        if (first) {
          first = false;
        } else {
          os << ",";
        }
        os << field.label;
      }
      return os;
    }

    friend inline std::string fmt_string(const flags_bitfield_fmt &obj) {
      std::stringstream os;
      os << obj;
      return os.str();
    }
  };

  flags_bitfield_fmt fmt() const {
    auto &t = static_cast<const Tag &>(*this);
    return flags_bitfield_fmt{t};
  }

  friend inline std::ostream &operator<<(std::ostream &os, Tag const &rhs) {
    os << rhs.fmt();
    return os;
  }

  friend inline std::string fmt_string(Tag const &rhs) {
    std::stringstream ss;
    ss << rhs;
    return ss.str();
  }
};

}  // namespace sled

template <typename Ta, typename Tb, typename Tag>
struct sled::__is_wrapped_integer_helper<sled::struct_bitfield<Ta, Tb, Tag>>
    : public std::true_type {};
