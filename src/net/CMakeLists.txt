add_library(sled-net
    epoll.cpp
    sock.cpp
    )

target_link_libraries(sled-net
    sled-exec)

add_unit_test(
    NAME sled-net-check
    SRC sock_test.cpp
    DEPS sled-net
    )
