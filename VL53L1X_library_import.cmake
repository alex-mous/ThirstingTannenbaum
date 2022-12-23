set(EXTERNAL_LIB_DIR "${PROJECT_SOURCE_DIR}/external_VL53L1X")

find_package(Git REQUIRED)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${EXTERNAL_LIB_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

include(${EXTERNAL_LIB_DIR}/VL53L1X_pico_api/CMakeLists.txt)
include_directories(${EXTERNAL_LIB_DIR}/VL53L1X_pico_api/)
link_directories(${EXTERNAL_LIB_DIR}/VL53L1X_pico_api/)