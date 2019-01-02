add_library(sled-lib
    base64.cpp
    log.cpp
    statistics.cpp
    )

add_unit_test(
    NAME lib-check
    SRC base64_test.cpp
        crc_test.cpp
        enum_test.cpp
        exception_test.cpp
        numeric_test.cpp
        log_test.cpp
        statistics_test.cpp
        time_test.cpp
    DEPS sled-lib
    )
