# This is a file that downloads the lwIP makefsdata Perl program
# and runs it on the public directory


# If on Windows, install Perl through Cygwin
# Otherwise, Perl likely available by default
find_package(Perl REQUIRED)

set(MAKE_FS_DATA_SCRIPT ${CMAKE_BINARY_DIR}/external/makefsdata)

# Download makefsdata script
if (NOT EXISTS ${MAKE_FS_DATA_SCRIPT})
        file(DOWNLOAD
                https://raw.githubusercontent.com/krzmaz/lwip/e15654409d14a238aec5ed4bd5516063938c9345/src/apps/http/makefsdata/makefsdata
                ${MAKE_FS_DATA_SCRIPT}
                )
endif()


message(${MAKE_FS_DATA_SCRIPT})

message("Running makefsdata script")
execute_process(COMMAND
        perl ${MAKE_FS_DATA_SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        ECHO_OUTPUT_VARIABLE
        ECHO_ERROR_VARIABLE
        COMMAND_ERROR_IS_FATAL ANY
        )

file(RENAME fsdata.c generated_fsdata.c)
