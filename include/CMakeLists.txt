message(STATUS "Conifguring include files")

configure_file(
    "${PROJECT_SOURCE_DIR}/include/sled/opts.h.in"
    "${PROJECT_BINARY_DIR}/include/sled/opts.h")
