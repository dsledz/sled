/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */
#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "sled/exception.h"
#include "sled/fmt.h"
#include "sled/numeric.h"
#include "sled/platform.h"

extern "C" {
#include "lua53/lauxlib.h"
#include "lua53/lua.h"
#include "lua53/lualib.h"
}

namespace sled::lua {
struct LuaException : public std::exception {
  const char *what() const noexcept override { return msg_.c_str(); }

 public:
  explicit LuaException(std::string msg) : msg_(std::move(msg)) {}

  template <typename... Args>
  explicit LuaException(Args... args) {
    std::stringstream ss;
    build(ss, args...);
    msg_ = ss.str();
  }

 protected:
  template <typename T>
  void build(std::stringstream &ss, T &&arg) {
    ss << std::forward<T>(arg);
  }

  template <typename T, typename... Args>
  void build(std::stringstream &ss, T &&arg, Args... args) {
    ss << std::forward<T>(arg);
    build(ss, args...);
  }

  std::string msg_;
};

class LuaTypeException : public LuaException {
 public:
  explicit LuaTypeException(std::string const &expected,
                            std::string const &actual)
      : LuaException("Expected: \"", expected, "\", actual: \"", actual, "\"") {
  }
};

// Overload set for type-safe callables
// For each function you wish to call from C++, write the corresponding
// func::function overload
static inline int lua_specialized_call(lua_State *L,
                                       const func::function<int(int)> &closure) {
  if (lua_gettop(L) != 1) {
    lua_pushstring(L, "Incorrect arguments, expected (int)");
    lua_error(L);
    return 0;
  }

  auto arg1 = static_cast<int>(lua_tonumber(L, -1));
  int result;
  result = closure(arg1);
  lua_pushnumber(L, result);
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L, const func::function<void(int)> &closure) {
  if (lua_gettop(L) != 1) {
    lua_pushstring(L, "Incorrect arguments, expected (int)");
    lua_error(L);
    return 0;
  }

  auto arg1 = static_cast<int>(lua_tonumber(L, -1));
  closure(arg1);
  lua_pushnil(L);
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L, const func::function<void(int, int)> &closure) {
  if (lua_gettop(L) != 2) {
    lua_pushstring(L, "Incorrect arguments, expected (int, int)");
    lua_error(L);
    return 0;
  }

  // LIFO
  auto arg2 = static_cast<int>(lua_tonumber(L, -1));
  auto arg1 = static_cast<int>(lua_tonumber(L, -2));
  closure(arg1, arg2);
  lua_pushnil(L);
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L, const func::function<int(int, int)> &closure) {
  if (lua_gettop(L) != 2) {
    lua_pushstring(L, "Incorrect arguments, expected (int, int)");
    lua_error(L);
    return 0;
  }

  // LIFO
  auto arg2 = static_cast<int>(lua_tonumber(L, -1));
  auto arg1 = static_cast<int>(lua_tonumber(L, -2));
  int result;
  result = closure(arg1, arg2);
  lua_pushnumber(L, result);
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L,
    const func::function<std::string(const std::string &)> &closure) {
  if (lua_gettop(L) != 1) {
    lua_pushstring(L, "Incorrect arguments, expected (string)");
    lua_error(L);
    return 0;
  }

  const char *arg1 = lua_tostring(L, -1);
  std::string result = closure(arg1);
  lua_pushstring(L, result.c_str());
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L,
    const func::function<std::string(const std::string &, const std::string &)>
        &closure) {
  if (lua_gettop(L) != 2) {
    lua_pushstring(L, "Incorrect arguments, expected (string)");
    lua_error(L);
    return 0;
  }

  // LIFO
  const char *arg2 = lua_tostring(L, -1);
  const char *arg1 = lua_tostring(L, -2);
  std::string result = closure(arg1, arg2);
  lua_pushstring(L, result.c_str());
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L,
    const func::function<void(const std::string &, const std::string &)>
        &closure) {
  if (lua_gettop(L) != 2) {
    lua_pushstring(L, "Incorrect arguments, expected (string, string)");
    lua_error(L);
    return 0;
  }

  // LIFO
  const char *arg2 = lua_tostring(L, -1);
  const char *arg1 = lua_tostring(L, -2);
  closure(arg1, arg2);
  lua_pushnil(L);
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L, const func::function<std::string(void)> &closure) {
  if (lua_gettop(L) != 0) {
    lua_pushstring(L, "Incorrect arguments, expected ()");
    lua_error(L);
    return 0;
  }

  std::string result = closure();
  lua_pushstring(L, result.c_str());
  return 1;
}

static inline int lua_specialized_call(
    lua_State *L, const func::function<void(void)> &closure) {
  if (lua_gettop(L) != 0) {
    lua_pushstring(L, "Incorrect arguments, expected ()");
    lua_error(L);
    return 0;
  }

  closure();
  lua_pushnil(L);
  return 1;
}

// End overload set

using lua_call_t = func::function<int(lua_State *)>;

/**
 * Wrapper around a lua callable as a value type.
 */
class LuaCall {
 public:
  LuaCall(std::string name, lua_call_t *call)
      : name_(std::move(name)), call_(call) {}
  ~LuaCall() = default;
  const std::string &name() { return name_; }
  lua_call_t *call() { return call_; }

 private:
  const std::string name_;
  lua_call_t *call_;
};

static inline int lua_callback_wrapper(lua_State *L) {
  void *var = lua_touserdata(L, lua_upvalueindex(1));
  auto call = static_cast<lua_call_t *>(var);
  return (*call)(L);
}

/**
 * Primary Lua Support.
 *
 * Represents a lua intepreter.
 */
class LuaSupport {
 public:
  LuaSupport();
  ~LuaSupport();

  LuaSupport(const LuaSupport &ls) = delete;

  // NOLINTNEXTLINE
  inline operator lua_State *() { return lua_.get(); }

  void add_callback(const std::string &name, lua_call_t *call) {
    auto lua = lua_.get();
    lua_pushlightuserdata(lua, call);
    lua_pushcclosure(lua, lua_callback_wrapper, 1);
    lua_setglobal(lua, name.c_str());
  }

  void add(LuaCall &call) { add_callback(call.name(), call.call()); }

  void add(const std::string &name, lua_CFunction func) {
    auto lua = lua_.get();
    lua_register(lua, name.c_str(), func);
  }

  void remove(const std::string & /*name*/) {
    // XXX:lua_pushnil(lua_);
    // XXX:lua_setglobal(lua_, name.c_str());
  }

  /**
   * Parse, execute and return the value.
   *
   * call_and_return<int>("return 5;");
   * call_and_return<std::string>("return 'example';");
   */
  template <class T>
  T call_and_return(const std::string &code);

  /**
   * Parse and execute code.
   *
   * Returns the lua errore code.
   */
  int call(const std::string &code) {
    auto lua = lua_.get();
    int status = luaL_loadstring(lua, code.c_str());
    if (status != 0) {
      throw LuaException(lua_tostring(lua, -1));
    }

    status = lua_pcall(lua, 0, LUA_MULTRET, 0);
    if (status != 0) {
      throw LuaException(lua_tostring(lua, -1));
    }

    return lua_gettop(lua);
  }

 private:
  void load_libraries();

 private:
  std::unique_ptr<lua_State, void (*)(lua_State *)> lua_;
};

template <>
inline int LuaSupport::call_and_return<int>(const std::string &code) {
  auto lua = lua_.get();
  int oldtop = lua_gettop(lua);
  int top = call(code);
  if (oldtop == top) {
    throw LuaTypeException("none", "int");
  }
  if (lua_isnumber(lua, top) == 0) {
    throw LuaTypeException("int", lua_typename(lua, top));
  }
  return static_cast<int>(lua_tointeger(lua, -1));
}

template <>
inline std::string LuaSupport::call_and_return<std::string>(
    const std::string &code) {
  auto lua = lua_.get();
  int oldtop = lua_gettop(lua);
  int top = call(code);
  std::string result;
  if (oldtop == top) {
    result = "";
  } else if (lua_isnoneornil(lua, top)) {
    result = "<nil>";
  } else if (lua_isnumber(lua, top) != 0) {
    auto ret = static_cast<int>(lua_tointeger(lua, top));
    result = sled::format(sled::IntFmt(ret));
  } else if (lua_isstring(lua, top) != 0) {
    const char *ret = lua_tostring(lua, top);
    result = std::string(ret);
  } else if (lua_isnil(lua, top)) {
    result = "";
  } else {
    throw LuaTypeException("string", lua_typename(lua, top));
  }
  lua_pop(lua, top);
  return result;
}

/**
 * Base LUA Object
 */
class LuaObject {
 public:
  virtual ~LuaObject() = default;
};

/**
 * Collection of LUA objects.
 */
class LuaObjects {
 public:
  LuaObjects() = default;
  ~LuaObjects() = default;

  void add(const std::string &name, std::unique_ptr<LuaObject> object) {
    m_objects.insert(make_pair(name, std::move(object)));
  }

 private:
  std::map<std::string, std::unique_ptr<LuaObject>> m_objects;
};

/**
 * Wraps a closure/callable and converts it into a lua specialized call.
 */
template <typename closure_t>
class LuaClosure : public LuaObject {
 public:
  LuaClosure(const std::string &name, closure_t closure)
      : m_func([=](lua_State *L) -> int {
          return lua_specialized_call(L, closure);
        }),
        m_call(name, &m_func) {}

  LuaClosure(const LuaClosure &) = delete;

  // NOLINTNEXTLINE
  inline operator LuaCall &() { return m_call; }

 private:
  lua_call_t m_func;
  LuaCall m_call;
};

struct LuaFunction : public LuaObject {
  LuaFunction(LuaSupport &lua, const std::string &name, lua_call_t *call)
      : m_lua(lua), m_name(name) {
    lua.add_callback(name, call);
  }
  ~LuaFunction() override { m_lua.remove(m_name); }

  LuaFunction(const LuaFunction &) = delete;

  LuaSupport &m_lua;
  const std::string m_name;
};

class LuaLibrary : public LuaObject {
 public:
  explicit LuaLibrary(const std::string &name)
      : m_name(name),

        m_library_open([=](lua_State *L) -> int { return library_open(L); }),
        m_call(name, &m_library_open) {}
  ~LuaLibrary() override = default;

  LuaLibrary(const LuaLibrary &) = delete;

  void add_call(LuaCall &call) { m_methods.push_back(call); }

  void add_static_call(LuaCall &call) { m_methods.push_back(call); }

  void remove_static_call(LuaCall &call) {
    // Caller must ensure that that library is closed before calling this method
    for (auto it = m_methods.begin(); it != m_methods.end(); it++) {
      if (it->name() == call.name()) {
        m_methods.erase(it);
        return;
      }
    }
  }

  template <typename closure_t>
  void add_callable(std::unique_ptr<LuaClosure<closure_t>> callable) {
    LuaCall &call = *callable;
    m_methods.push_back(call);
    std::unique_ptr<LuaObject> obj(std::move(callable));
    m_objects.add(call.name(), std::move(obj));
  }

  template <typename callable_t>
  void add_callable(const std::string &name, callable_t callable) {
    add_callable(std::make_unique<LuaClosure<callable_t>>(name, callable));
  }

  // NOLINTNEXTLINE
  inline operator LuaCall &() { return m_call; }

  void finalize_library(LuaSupport &lua) {
    lua.add_callback(m_name, &m_library_open);
  }

 protected:
  int library_open(lua_State *L) {
    lua_newtable(L);
    for (auto call : m_methods) {
      lua_pushlightuserdata(L, call.call());
      lua_pushcclosure(L, lua_callback_wrapper, 1);
      lua_setfield(L, -2, call.name().c_str());
    }
    return 1;
  }

  LuaObjects m_objects;
  const std::string m_name;
  std::list<LuaCall> m_methods;
  lua_call_t m_library_open;
  LuaCall m_call;
  friend int lua_library_open(lua_State *L);
};

class LuaClass : public LuaObject {
 public:
  explicit LuaClass(const std::string &name)
      : m_name(name),

        m_register_class(
            [=](lua_State *L) -> int { return register_class(L); }),
        m_meta_gc([=](lua_State *L) -> int { return gc(L); }),
        m_meta_index([=](lua_State *L) -> int { return index(L); }),
        m_meta_newindex([=](lua_State *L) -> int { return newindex(L); }),
        m_call(name, &m_register_class) {
    m_methods.emplace_back("__gc", &m_meta_gc);
    m_methods.emplace_back("__index", &m_meta_index);
    m_methods.emplace_back("__newindex", &m_meta_newindex);
  }

  ~LuaClass() override = default;

  LuaClass(const LuaClass &) = delete;

  void add_call(LuaCall &call) { m_methods.push_back(call); }

  void add_static_call(LuaCall &call) { m_methods.push_back(call); }

  void remove_static_call(LuaCall &call) {
    // Caller must ensure that that class is closed before calling this method
    for (auto it = m_methods.begin(); it != m_methods.end(); it++) {
      if (it->name() == call.name()) {
        m_methods.erase(it);
        return;
      }
    }
  }

  template <typename closure_t>
  void add_callable(std::unique_ptr<LuaClosure<closure_t>> callable) {
    LuaCall &call = *callable;
    m_methods.push_back(call);
    std::unique_ptr<LuaObject> obj(std::move(callable));
    m_objects.add(call.name(), std::move(obj));
  }

  template <typename callable_t>
  void add_callable(const std::string &name, callable_t callable) {
    add_callable(std::make_unique<LuaClosure<callable_t>>(name, callable));
  }

  // NOLINTNEXTLINE
  inline operator LuaCall &() { return m_call; }

 protected:
  virtual bool lua_index(const std::string & /*k*/, int & /*v*/) {
    return false;
  }

  virtual bool lua_index(const std::string & /*k*/, std::string & /*v*/) {
    return false;
  }

  virtual bool lua_newindex(const std::string & /*k*/, const int /*v*/) {
    return false;
  }

  virtual bool lua_newindex(const std::string & /*k*/,
                            const std::string & /*v*/) {
    return false;
  }

  int register_class(lua_State *L) {
    // newclass = {}
    lua_createtable(L, 0, 0);
    {
      int lib_id;
      lib_id = lua_gettop(L);

      // metatable = {}
      luaL_newmetatable(L, m_name.c_str());
      {
        int meta_id = lua_gettop(L);

        // metatable.__index =
        lua_pushlightuserdata(L, &m_meta_index);
        lua_pushcclosure(L, lua_callback_wrapper, 1);
        lua_setfield(L, meta_id, "__index");

        // metatable.__newindex =
        lua_pushlightuserdata(L, &m_meta_newindex);
        lua_pushcclosure(L, lua_callback_wrapper, 1);
        lua_setfield(L, meta_id, "__newindex");

        // metatable.__metatable = {..}
        lua_newtable(L);
        {
          lua_pushlightuserdata(L, &m_meta_gc);
          lua_pushcclosure(L, lua_callback_wrapper, 1);
          lua_setfield(L, -2, "__gc");
          lua_pushlightuserdata(L, &m_meta_index);
          lua_pushcclosure(L, lua_callback_wrapper, 1);
          lua_setfield(L, -2, "__index");
          lua_pushlightuserdata(L, &m_meta_newindex);
          lua_pushcclosure(L, lua_callback_wrapper, 1);
          lua_setfield(L, -2, "__newindex");

          lua_setfield(L, meta_id, "__metatable");
        }

        // class.__metatable = metatable
        lua_setmetatable(L, lib_id);
      }

      // return newclass
    }

    return 1;
  }

  int gc(lua_State * /*L*/) {
    abort();
    return 1;
  }

  int index(lua_State *L) {
    if (lua_gettop(L) != 2) {
      lua_pushstring(L, "Incorrect arguments");
      lua_error(L);
      return 0;
    }
    bool b = false;
    const std::string &key(lua_tostring(L, -1));
    // Make sure they're not trying to update one of our methods
    for (auto &method : m_methods) {
      if (key == method.name()) {
        lua_pushlightuserdata(L, method.call());
        lua_pushcclosure(L, lua_callback_wrapper, 1);
        return 1;
      }
    }

    int ivalue = 0;
    b = lua_index(key, ivalue);
    if (b) {
      lua_pushnumber(L, ivalue);
      return 1;
    }

    std::string svalue;
    b = lua_index(key, svalue);
    if (b) {
      lua_pushstring(L, svalue.c_str());
      return 1;
    }

    std::string t = "Failed to read '" + key + "'";
    lua_pushstring(L, t.c_str());
    lua_error(L);
    return 0;
  }

  int newindex(lua_State *L) {
    if (lua_gettop(L) != 3) {
      lua_pushstring(L, "Incorrect arguments");
      lua_error(L);
      return 0;
    }

    bool b = false;
    const std::string &key(lua_tostring(L, -2));
    // Make sure they're not trying to update one of our methods
    for (auto &method : m_methods) {
      if (key == method.name()) {
        lua_pushstring(L, "Attempting to update built-in method");
        lua_error(L);
        return 0;
      }
    }
    if (lua_isnumber(L, -1) != 0) {
      auto ivalue = static_cast<int>(lua_tonumber(L, -1));
      b = lua_newindex(key, ivalue);
    } else if (lua_isstring(L, -1) != 0) {
      const char *svalue = lua_tostring(L, -1);
      b = lua_newindex(key, svalue);
    }
    if (!b) {
      std::string t = "Failed to write '" + key + "'";
      lua_pushstring(L, t.c_str());
      lua_error(L);
      return 0;
    }
    return 1;
  }

  LuaObjects m_objects;
  const std::string m_name;
  std::list<LuaCall> m_methods;
  lua_call_t m_register_class;
  lua_call_t m_meta_gc;
  lua_call_t m_meta_index;
  lua_call_t m_meta_newindex;
  LuaCall m_call;
  friend int lua_library_open(lua_State *L);
};

}  // namespace sled::lua
