message(STATUS "Building 3rd party dependencies")

if (SLED_BUILD_GTEST)
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
endif (SLED_BUILD_GTEST)
