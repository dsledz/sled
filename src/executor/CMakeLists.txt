if (WIN32)
    ENABLE_LANGUAGE(ASM_MASM)
    set(SUPPORT_ASM win64.asm)
else (WIN32)
    ENABLE_LANGUAGE(ASM-ATT)
    if (UNIX)
        set(SUPPORT_ASM linux_asm.s)
    endif (UNIX)
    if (APPLE)
        set(SUPPORT_ASM osx_asm.s)
    endif (APPLE)
    if (FREEBSD)
        set(SUPPORT_ASM freebsd_asm.s)
    endif (FREEBSD)
endif (WIN32)

add_library(sled-exec
    coroutine.cpp
    task.cpp
    threadpool.cpp
    ${SUPPORT_ASM})

target_link_libraries(sled-exec
    sled-lib)

add_unit_test(
    NAME executor-check
    SRC channel_test.cpp
        coroutine_test.cpp
        executor_mock.cpp
        future_test.cpp
        task_test.cpp
        threadpool_test.cpp
    DEPS sled-exec)
