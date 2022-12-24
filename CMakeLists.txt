cmake_minimum_required(VERSION 3.25)

option(PRINT_STDIO "Print debug messages, warnings, and errors to USB stdio" ON)

# Only works with Pico W
set(PICO_BOARD pico_w)

# Pull in SDK by downloading the pico_sdk_import
set(PICO_SDK_IMPORT ${CMAKE_BINARY_DIR}/external/pico_sdk_import.cmake)
if (NOT EXISTS ${PICO_SDK_IMPORT})
        file(DOWNLOAD
                https://raw.githubusercontent.com/raspberrypi/pico-sdk/2e6142b15b8a75c1227dd3edbe839193b2bf9041/external/pico_sdk_import.cmake
                ${PICO_SDK_IMPORT}
                )
endif()
include(${PICO_SDK_IMPORT})

# Make fs data
include(make_fs.cmake)

# Setup project info
project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Initialize the SDK
pico_sdk_init()

# Add VL53L1X lib
include(${PROJECT_SOURCE_DIR}/external_VL53L1X/library/import.cmake)

# Show messages on STDIO
if (PRINT_STDIO)
    add_definitions(-DPRINT_STDIO)
endif()

# Compile executable (link libs)
add_executable(main
    main.cpp
    status.cpp
    io.cpp
    ssi_server.cpp)

target_link_libraries(main PUBLIC
    pico_stdlib
    hardware_i2c
    VL53L1X_pico_api
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_multicore
    pico_util)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..)

pico_add_extra_outputs(main)

if (PRINT_STDIO)
    pico_enable_stdio_usb(main 1)
endif()