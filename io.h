#ifndef _IO_H_
#define _IO_H_


#define LED_R_PIN 11
#define LED_G_PIN 12
#define LED_B_PIN 13

// Init LEDs (turns off)
void leds_init();

// Set the LEDs to the color
void leds_set(int r, int g, int b);

#endif  // _IO_H_