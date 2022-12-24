#include <string>
#include <iostream>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "io.h"
#include "status.h"


void status_log(string msg) {
#ifdef PRINT_STDIO
    std::cout << "[LOG] " << msg << std::endl;
#endif
}

void status_warn(string msg) {
#ifdef PRINT_STDIO
    std::cout << "[WARN] " << msg << std::endl;
#endif
    flash_leds(WARN_MS, WARN_MS, WARN_REPEATS);
}

void status_error(string msg) {
#ifdef PRINT_STDIO
    std::cout << "[ERROR] " << msg << std::endl;
#endif
    flash_leds(ERROR_MS, ERROR_MS, ERROR_REPEATS);
}

void status_fatal(string msg) {
#ifdef PRINT_STDIO
    std::cout << "[FATAL] " << msg << std::endl;
#endif
    flash_leds(FATAL_MS, FATAL_MS, FATAL_REPEATS);
    exit(1);
}
