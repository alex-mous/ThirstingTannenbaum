/**
 * @file main.c
 * @author Alex Mous
 * @brief Main program
 * @version 0.1
 * @date 2022-11-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "distance.h"
#include "bluetooth.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define SW_PIN 28
#define BUZZER_PIN 7

int log_routine();
int fill_routine();

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return EXIT_FAILURE;
    }

    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
        printf("Testing connection\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
    }

    /*gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);*/

    /*while (1) {
        if ()
    }

    */
    /*bluetooth_init();*/

    /*while (1) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(1000);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        //double d = measure_dist();
        printf("Measuring\r\n");
        //bluetooth_send(d);
        sleep_ms(1000);
    }*/
}