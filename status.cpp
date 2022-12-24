#include <string>
#include <iostream>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "status.h"


void flash_leds(int ontime, int offtime, int repeats) {
    for (int i=0; i<repeats; i++) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(ontime);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(offtime);
    }
}

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
