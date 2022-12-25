#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/pwm.h"

#include "io.h"


void flash_leds(uint16_t on_ms, uint16_t off_ms, uint16_t reps) {
    for (uint16_t i=0; i<reps; i++) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(on_ms);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(off_ms);
    }
}

void leds_init() {
    gpio_set_function(LED_R_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_G_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_B_PIN, GPIO_FUNC_PWM);
    
    uint slice_num_r = pwm_gpio_to_slice_num(LED_R_PIN);
    uint slice_num_g = pwm_gpio_to_slice_num(LED_G_PIN);
    uint slice_num_b = pwm_gpio_to_slice_num(LED_B_PIN);

    pwm_set_enabled(slice_num_r, true);
    pwm_set_enabled(slice_num_g, true);
    pwm_set_enabled(slice_num_b, true);

    leds_set(0, 0, 0);
}

void leds_set(uint8_t r, uint8_t g, uint8_t b) {
    pwm_set_gpio_level(LED_R_PIN, 255*r);
    pwm_set_gpio_level(LED_G_PIN, 255*g);
    pwm_set_gpio_level(LED_B_PIN, 255*b);
}

void buzzer_init() {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

void buzzer_pulse(uint16_t on_ms, uint16_t off_ms, uint16_t reps) {
    for (uint16_t i=0; i<reps; i++) {
        gpio_put(BUZZER_PIN, 1);
        sleep_ms(on_ms);
        gpio_put(BUZZER_PIN, 0);
        sleep_ms(off_ms);
    }
}

void switch_init(gpio_irq_callback_t cb) {
    gpio_init(SW_R_PIN);
    gpio_set_dir(SW_R_PIN, GPIO_IN);
    gpio_pull_down(SW_R_PIN);
    gpio_init(SW_W_PIN);
    gpio_set_dir(SW_W_PIN, GPIO_OUT);
    gpio_put(SW_W_PIN, 1);
    gpio_set_irq_enabled_with_callback(SW_R_PIN, GPIO_IRQ_EDGE_RISE, true, cb);
}