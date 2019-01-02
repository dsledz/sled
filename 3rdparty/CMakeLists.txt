message(STATUS "Building 3rd party dependencies")

if (BUILD_GTEST)
    message(STATUS "Building GTest via external Project")
    if (REBUILD_3RDPARTY)
        # Download and unpack googletest at configure time
        configure_file(gtest-CMakeLists.txt.in googletest-download/CMakeLists.txt)
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
        if(result)
            message(FATAL_ERROR "CMake step for googletest failed: ${result}")
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
        if(result)
            message(FATAL_ERROR "Build step for googletest failed: ${result}")
        endif()
        set(REBUILD_3RDPARTY OFF CACHE BOOL "" FORCE)
    endif (REBUILD_3RDPARTY)

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
		     ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
		     EXCLUDE_FROM_ALL)

    set(GTEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest-src)
    include_directories(${GTEST_DIR}/include)

    set_target_properties(gtest PROPERTIES FOLDER 3rdParty)
    set_target_properties(gtest_main PROPERTIES FOLDER 3rdParty)
    set_target_properties(gmock PROPERTIES FOLDER 3rdParty)
    set_target_properties(gmock_main PROPERTIES FOLDER 3rdParty)
endif (BUILD_GTEST)

if (BUILD_LUA)
    message(STATUS "Building LUA 5.3")
    add_library(lua53
        lua53/lapi.c
        lua53/lcode.c
        lua53/lctype.c
        lua53/ldebug.c
        lua53/ldo.c
        lua53/ldump.c
        lua53/lfunc.c
        lua53/lgc.c
        lua53/llex.c
        lua53/lmem.c
        lua53/lobject.c
        lua53/lopcodes.c
        lua53/lparser.c
        lua53/lstate.c
        lua53/lstring.c
        lua53/ltable.c
        lua53/ltm.c
        lua53/lundump.c
        lua53/lvm.c
        lua53/lzio.c
        lua53/lauxlib.c
        lua53/lbaselib.c
        lua53/lbitlib.c
        lua53/lcorolib.c
        lua53/ldblib.c
        lua53/liolib.c
        lua53/lmathlib.c
        lua53/loslib.c
        lua53/lstrlib.c
        lua53/ltablib.c
        lua53/lutf8lib.c
        lua53/loadlib.c
        lua53/linit.c)

    include_directories(lua53)

    if(UNIX)
        add_definitions(-DLUA_USE_POSIX)
        target_link_libraries(lua53
            m)
        cotire(lua53)
    ENDIF(UNIX)
endif (BUILD_LUA)

