/*
 * Copyright (c) 2013-2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/lua.h"

#include "gtest/gtest.h"

extern "C" {
#include "lua53/lauxlib.h"
#include "lua53/lua.h"
#include "lua53/lualib.h"
}

class LuaTest : public ::testing::Test {
 protected:
  LuaTest() = default;

  sled::lua::LuaSupport lua;
};

TEST_F(LuaTest, custom_function) {
  std::string script = R"(
-- script.lua
-- Receives a table, returns the sum of its components.
x = 0
for i = 1, #foo do
    x = x + foo[i]
end
return x)";

  int status;
  status = luaL_loadstring(lua, script.c_str());
  EXPECT_EQ(status, 0);

  lua_newtable(lua);
  for (int i = 0; i < 5; i++) {
    lua_pushnumber(lua, i);
    lua_pushnumber(lua, i * 2);
    lua_rawset(lua, -3);
  }
  lua_setglobal(lua, "foo");

  status = lua_pcall(lua, 0, LUA_MULTRET, 0);
  EXPECT_EQ(status, 0);

  auto sum = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(sum, 20);
  lua_pop(lua, -1);
}

int test_external_function(lua_State *L) {
  int n = lua_gettop(L);
  auto value = static_cast<int>(lua_tointeger(L, -1));
  value *= 2 * n;
  lua_pushnumber(L, value);
  return 1;
}

TEST_F(LuaTest, external_function) {
  std::string script = R"(
-- Call a function
x = double(5)
return x)";

  lua.add("double", test_external_function);

  lua.call(script);

  auto ret = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(ret, 10);
  lua_pop(lua, -1);
}

TEST_F(LuaTest, external_call_object) {
  std::string script = R"(
-- Call a function
x = double(5)
return x)";
  std::string name = "double";
  sled::lua::lua_call_t func = test_external_function;
  sled::lua::LuaCall call(name, &func);

  lua.add(call);
  lua.call(script);

  auto ret = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(ret, 10);
  lua_pop(lua, -1);
}

int test_external_function_failure(lua_State *L) {
  int n = lua_gettop(L);
  if (n != 1) {
    lua_pushstring(L, "Incorrect number of arguments");
    lua_error(L);
  }
  return 0;
}

TEST_F(LuaTest, external_function_failure) {
  std::string script = R"(
-- Error check
x = failure(5, 5)
return x)";

  lua.add("failure", test_external_function_failure);
  EXPECT_THROW(lua.call(script), sled::lua::LuaException);
}

int test_lua_call(lua_State *L) {
  int n = lua_gettop(L);
  auto value = static_cast<int>(lua_tointeger(L, -1));
  value *= 2 * n;
  lua_pushnumber(L, value);
  return 1;
}

TEST_F(LuaTest, call_and_return_int) {
  std::string script = R"(
    return 1)";

  EXPECT_EQ(1, lua.call_and_return<int>(script));
}

TEST_F(LuaTest, call_and_return_string) {
  std::string script = R"(
    return "test")";

  EXPECT_EQ("test", lua.call_and_return<std::string>(script));
}

TEST_F(LuaTest, call_and_return_int_as_string) {
  std::string script = R"(
    return 1)";

  EXPECT_EQ("1", lua.call_and_return<std::string>(script));
}

TEST_F(LuaTest, closure_test) {
  sled::lua::LuaClosure<std::function<int(int)> > closure(
      "test", [](int v) -> int { return v * 5; });
  lua.add(closure);

  std::string script = R"(
        return test(5)
    )";

  lua.call(script);

  auto ret = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(ret, 25);
  lua_pop(lua, -1);
}

TEST_F(LuaTest, closure_test_a_b) {
  sled::lua::LuaClosure<std::function<int(int, int)> > closure(
      "test", [](int a, int b) -> int { return b; });
  lua.add(closure);

  std::string script = R"(
        return test(1,2)
    )";

  lua.call(script);

  auto ret = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(ret, 2);
  lua_pop(lua, -1);
}

TEST_F(LuaTest, closure_string_return_string) {
  sled::lua::LuaClosure<std::string(const std::string &)> closure(
      "test", [](const std::string &v) -> std::string { return v + "_end"; });
  lua.add(closure);

  std::string script = R"(
        x = test("start")
        return x
    )";

  lua.call(script);

  std::string ret = lua_tostring(lua, -1);
  EXPECT_EQ(ret, "start_end");
  lua_pop(lua, -1);
}

TEST_F(LuaTest, closure_void_return_string) {
  sled::lua::LuaClosure<std::function<std::string(void)> > closure(
      "test", []() -> std::string { return "end"; });
  lua.add(closure);

  std::string script = R"(
        x = test()
        return x
    )";

  lua.call(script);

  std::string ret = lua_tostring(lua, -1);
  EXPECT_EQ(ret, "end");
  lua_pop(lua, -1);
}

TEST_F(LuaTest, closure_exception) {
  sled::lua::LuaClosure<std::function<std::string(void)> > closure(
      "test", []() -> std::string { return "end"; });
  lua.add(closure);

  std::string script = R"(
        x = test("start")
        return x
    )";

  EXPECT_THROW(lua.call(script), sled::lua::LuaException);
}

TEST_F(LuaTest, library_test) {
  sled::lua::LuaClosure<std::function<int(int)> > inc(
      "inc", [](int v) -> int { return v + 1; });
  sled::lua::LuaClosure<std::function<int(int)> > dec(
      "dec", [](int v) -> int { return v - 1; });
  sled::lua::LuaLibrary lib("ops");

  lib.add_call(inc);
  lib.add_call(dec);

  lib.finalize_library(lua);

  std::string script = R"(
        lib = ops()
        x = 10
        x = lib.inc(x)
        x = lib.inc(x)
        x = lib.dec(x)
        return x
    )";

  lua.call(script);

  EXPECT_EQ(1, lua_isnumber(lua, -1));
  auto ret = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(ret, 11);
  lua_pop(lua, -1);
}

TEST_F(LuaTest, class_test) {
  sled::lua::LuaClosure<std::function<int(int)> > inc(
      "inc", [](int v) -> int { return v + 1; });
  sled::lua::LuaClosure<std::function<int(int)> > dec(
      "dec", [](int v) -> int { return v - 1; });
  sled::lua::LuaClass cls("test");

  cls.add_call(inc);
  cls.add_call(dec);

  std::string script = R"(
      cls = test()
      x = 10
      x = cls.inc(x)
      x = cls.inc(x)
      x = cls.dec(x)
      return x
  )";

  lua.add(cls);
  lua.call(script);

  EXPECT_EQ(1, lua_isnumber(lua, -1));
  auto ret = static_cast<int>(lua_tointeger(lua, -1));
  EXPECT_EQ(ret, 11);
  lua_pop(lua, -1);
}
