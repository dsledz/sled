add_library(sled-lib
    base64.cpp
    cmdline.cpp
    log.cpp
    statistics.cpp
    )

add_unit_test(
    NAME sled-lib-check
    SRC base64_test.cpp
        bitfield_test.cpp
        bytestream_test.cpp
        cmdline_test.cpp
        crc_test.cpp
        enum_test.cpp
        exception_test.cpp
        fmt_test.cpp
        numeric_test.cpp
        log_test.cpp
        statistics_test.cpp
        string_test.cpp
        strong_int_test.cpp
        time_test.cpp
        union_test.cpp
        x86arch_test.cpp
    DEPS sled-lib
    )
