/*
 * Copyright (c) 2018, Dan Sledz
 * All rights reserved.
 * Licensed under BSD-2-Clause license.
 */

#include "sled/lua.h"

namespace sled::lua {

LuaSupport::LuaSupport() : lua_(luaL_newstate(), lua_close) {
  load_libraries();
}

LuaSupport::~LuaSupport() = default;

void LuaSupport::load_libraries() {
  luaopen_io(lua_.get());
  luaopen_base(lua_.get());
  luaopen_table(lua_.get());
  luaopen_string(lua_.get());
  luaopen_math(lua_.get());
  luaL_openlibs(lua_.get());
}

}
