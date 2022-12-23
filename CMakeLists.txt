cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

# Make fs data
include(make_fs.cmake)
# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)



# Pull in VL53L1X lib
include(vl53l1x_import.cmake)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

# Add VL53L1X lib

# Code

add_executable(test_ssi
    test_ssi.c)
target_link_libraries(test_ssi
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    VL53L1X_pico_api
    )

target_include_directories(test_ssi PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
    )

pico_add_extra_outputs(test_ssi)

pico_enable_stdio_usb(test_ssi 1)