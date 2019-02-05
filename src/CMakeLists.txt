message(STATUS "Configuring source...")

add_subdirectory(lib)
if (SLED_BUILD_LUA)
    add_subdirectory(lua)
endif (SLED_BUILD_LUA)
add_subdirectory(executor)
add_subdirectory(net)
