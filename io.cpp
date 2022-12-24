#include "pico/stdlib.h"
#include "io.h"

void leds_init() {
    // Configure GPIO
    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);

    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    //gpio_set_irq_enabled_with_callback()
}

void leds_set(int r, int g, int b) {
    // do pwm here
}


