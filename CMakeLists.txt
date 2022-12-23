cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

# Make fs data
include(make_fs.cmake)

# Pull in SDK
include(pico_sdk_import.cmake)

# Setup project info
project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wno-format -Wno-unused-function -Wno-maybe-uninitialized)

# Initialize the SDK
pico_sdk_init()

# Add VL53L1X lib
include(${PROJECT_SOURCE_DIR}/external_VL53L1X/library/import.cmake)

# Compile executable (link libs)
add_executable(test_ssi
    test_ssi.c)

target_link_libraries(test_ssi
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    VL53L1X_pico_api)

target_include_directories(test_ssi PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..)

pico_add_extra_outputs(test_ssi)
pico_enable_stdio_usb(test_ssi 1)