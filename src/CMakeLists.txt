message(STATUS "Configuring source...")

add_subdirectory(lib)
if (BUILD_LUA)
    add_subdirectory(lua)
endif (BUILD_LUA)
add_subdirectory(executor)
