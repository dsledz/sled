add_library(sled-lua
    lua.cpp
    )

target_link_libraries(sled-lua
    lua53 sled-lib)

add_unit_test(
    NAME lua-check
    SRC lua_test.cpp
    DEPS sled-lua)
