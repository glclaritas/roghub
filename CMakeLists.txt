cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rogctl VERSION 1.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
    src/main.c
    src/cpucontrol.c
    src/fancontrol.c
    src/profile.c
    src/osd.c
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)

add_executable(rogctl ${SOURCES})

target_include_directories(rogctl PRIVATE
    src
    ${CMAKE_CURRENT_BINARY_DIR}
)

install(FILES 99-roghub-control.rules
    DESTINATION lib/udev/rules.d)

install(TARGETS rogctl
        RUNTIME DESTINATION bin)
