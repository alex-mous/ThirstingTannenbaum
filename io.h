#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

// Define pin information (GPIO numbers)
#define LED_R_PIN 9
#define LED_G_PIN 8
#define LED_B_PIN 7

#define SW_PIN 6

#define BUZZER_PIN 11


// Flash onboard LED <repeats> times, on for <ontime> (ms), off for <offtime> (ms)
void flash_leds(uint16_t on_ms, uint16_t off_ms, uint16_t reps);

// Initialize the multicolor LEDs (turns off)
void leds_init();

// Set the LEDs to the color
void leds_set(uint8_t r, uint8_t g, uint8_t b);

// Initialize the buzzer (turns off)
void buzzer_init();

// Pulse the buzzer <repeats> times, on for <ontime> (ms), off for <offtime> (ms)
void buzzer_pulse(uint16_t on_ms, uint16_t off_ms, uint16_t reps);

// Initialize the switch to callback to cb on rising
void switch_init(gpio_irq_callback_t cb);
#endif  // _IO_H_