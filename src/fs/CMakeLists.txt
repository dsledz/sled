add_library(sled-fs
    file.cpp
    )

target_link_libraries(sled-fs
    sled-exec
    stdc++fs
    )

add_unit_test(
    NAME sled-fs-check
    SRC file_test.cpp
    DEPS sled-fs
    )
