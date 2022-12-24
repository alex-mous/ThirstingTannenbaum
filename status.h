// Define status reporting functions
// Prints messages if PRINT_STDIO is defined
// Shows error flash code on onboard LED

#ifndef _STATUS_H_
#define _STATUS_H_

#include <string>

// Define LED codes
#define WARN_MS 50
#define WARN_REPEATS 2

#define ERROR_MS 250
#define ERROR_REPEATS 3

#define FATAL_MS 500
#define FATAL_REPEATS 4

using std::string;

// Flash onboard LED <repeats> times, on for <ontime> (ms), off for <offtime> (ms)
void flash_leds(int ontime, int offtime, int repeats);

// Log a message to STDIO
void status_log(string msg);

// Warning message to STDIO and LEDs
void status_warn(string msg);

// Error message to STDIO and LEDs
void status_error(string msg);

// Fatal message to STDIO and LEDs, and halt program
void status_fatal(string msg);

#endif  // _STATUS_H_