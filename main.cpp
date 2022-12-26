#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <sstream>

// Pico Libraries
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"

// VL53L1X
extern "C" {
  #include "VL53L1X_types.h"
  #include "VL53L1X_api.h"
}

// WIFI and Server
#include "server.h"
#include "credentials.h"

// I/O
#include "io.h"

// Helpers
#include "status.h"


// Interface/device constants
#define WIFI_CONNECT_TIMEOUT_MS 5000
#define VL53L1X_ADDR 0x29
#define POLL_DELAY_MS 500

// Define min
#define min(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
// Define max
#define max(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b); _a > _b ? _a : _b; })


// raw sensor measurement in mm (1 element queue)
queue_t raw_level_mm;

// min, max level
uint16_t min_level, max_level;

// storage in flash for settings (min/max level)
const uint8_t *settings_flash_contents = (const uint8_t*) (XIP_BASE + SETTINGS_FLASH_OFFSET);


// Start the webserver. Blocks until connection is established.
void start_server();
// Initialize the VL53L1X sensor
VL53L1X_Status_t measurement_init();
// Continuously measure distance from the VL53L1X sensor
void measurement_core();
// Callback to run filling mode
void filling_mode();
// Get the percentage full (and adjust the bounds if needed)
double get_fill_percent(uint16_t level);

int main() {
  // stdio/board init
  stdio_init_all();
  if (cyw43_arch_init()) {
    status_fatal("Failed to start CYW43.");
  }

  // Init GPIO devices
  leds_init();
  buzzer_init();
  leds_set(0, 0, 255); // Blue while booting
  buzzer_pulse(25, 0, 1);

  gpio_init(SW_R_PIN);
  gpio_set_dir(SW_R_PIN, GPIO_IN);
  gpio_pull_down(SW_R_PIN);
  gpio_init(SW_W_PIN);
  gpio_set_dir(SW_W_PIN, GPIO_OUT);
  gpio_put(SW_W_PIN, 1);

  // Flash LEDs to show successful start
  status_warn("Starting Pico...");
  start_server();

  // Read min/max values from flash
  min_level = *((uint16_t*)settings_flash_contents);
  max_level = *(1 + (uint16_t*)settings_flash_contents);
  std::stringstream ss;
  ss << "Min = " << min_level << ". Max = " << max_level << "." << std::endl;
  status_log(ss.str());

  // Initialize queue
  queue_init(&raw_level_mm, sizeof(uint16_t), 1);

  // Initialize VL53L1X
  measurement_init();

  // Start continuous measurement
  multicore_launch_core1(measurement_core);


  // TODO:
  // --- add buzzer functionality (errors, startup, fillingmode, low??)
  // --- write filling mode (signal start with buzzer, etc)

  //min_level = 20;
  //max_level = 200;
  //write_settings_flash();
  //sleep_ms(2000);

  // Initialize the needed hardware
  while (1) {
    //uint16_t curr_level_mm;
    //queue_peek_blocking(&raw_level_mm, &curr_level_mm);
    //std::stringstream ss;
    //ss << "Packet received." << ", dist = " << get_fill_percent(curr_level_mm) << ", gpio = " << gpio_get(SW_R_PIN) << std::endl;
    //status_log(ss.str());
    if (gpio_get(SW_R_PIN) == 1)
      filling_mode();
    sleep_ms(POLL_DELAY_MS);
  }
}



void start_server() {
  // Connect to the WIFI network
  cyw43_arch_enable_sta_mode();
  while (1) {
    status_log("Connecting to WiFi...");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PSK, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS)) {
      status_warn("Failed to connect. Retrying in 5s...");
      sleep_ms(WIFI_CONNECT_TIMEOUT_MS);
    } else {
      status_log("Connected. Starting server at " + std::string(ip4addr_ntoa(netif_ip4_addr(netif_list))));
      break;
    }
  }

  // Start the webserver
  httpd_init();
  server_init();
  status_log("HTTP webserver initialized.");
}

VL53L1X_Status_t measurement_init() {
  VL53L1X_Status_t status;
  // Initialize Pico's I2C using PICO_DEFAULT_I2C_SDA_PIN
  // and PICO_DEFAULT_I2C_SCL_PIN (GPIO 4 and GPIO 5, respectively)
  if (VL53L1X_I2C_Init(VL53L1X_ADDR, i2c0) < 0) {
    status_fatal("Error initializing sensor.\n");
  }

  // Ensure the sensor has booted
  uint8_t sensorState;
  do {
    status += VL53L1X_BootState(VL53L1X_ADDR, &sensorState);
    VL53L1X_WaitMs(VL53L1X_ADDR, 2);
  } while (sensorState == 0);
  status_log("Sensor booted.\n");

  // Initialize and configure sensor
  status = VL53L1X_SensorInit(VL53L1X_ADDR);
  status += VL53L1X_SetDistanceMode(VL53L1X_ADDR, 1);
  status += VL53L1X_SetTimingBudgetInMs(VL53L1X_ADDR, 100);
  status += VL53L1X_SetInterMeasurementInMs(VL53L1X_ADDR, 100);
  status += VL53L1X_StartRanging(VL53L1X_ADDR);
  return status;
}

void measurement_core() {
  VL53L1X_Status_t status;
  VL53L1X_Result_t results;

  // Allow this to be paused
  multicore_lockout_victim_init();

  // Measure and print continuously
  bool first_range = true;
  while (1) {
    // Wait until we have new data
    uint8_t dataReady;
    do {
      status = VL53L1X_CheckForDataReady(VL53L1X_ADDR, &dataReady);
      sleep_us(1);
    } while (dataReady == 0);

    // Read, store, and display result
    status += VL53L1X_GetResult(VL53L1X_ADDR, &results);

    // Set LEDs based on value percentage
    double percent = get_fill_percent(results.distance);
    leds_set(min(255*(1-percent), 255), min(255*(percent), 255), 0);

    // Remove old value (after first run) and add new value
    if (!first_range)
      queue_remove_blocking(&raw_level_mm, nullptr);
    queue_add_blocking(&raw_level_mm, reinterpret_cast<void*>(&results.distance));

    // Clear the sensor for a new measurement
    status += VL53L1X_ClearInterrupt(VL53L1X_ADDR);
    if (first_range) {  // Clear twice on first measurement
      status += VL53L1X_ClearInterrupt(VL53L1X_ADDR);
      first_range = false;
    }
  }
}

void filling_mode() {
  sleep_ms(25);  // debounce
  if (gpio_get(SW_R_PIN) != 1)
    return;
  status_log("Start Filling Mode");
  buzzer_pulse(500, 500, 1);  // 1 long beep to indicate start
  while (gpio_get(SW_R_PIN) == 1) {  // Run while switch is enabled
    uint16_t curr_level_mm;
    queue_peek_blocking(&raw_level_mm, &curr_level_mm);
    double percent = get_fill_percent(curr_level_mm);
    buzzer_pulse(50*(percent)+50, 250*(1-percent), 1);// beep to indicate level
  }
  sleep_ms(500);
  status_log("End Filling Mode");
  buzzer_pulse(500, 500, 1);  // 1 long beep to indicate end
}

double get_fill_percent(uint16_t level) {
  return max(0, min(1.0, (max_level - level) / (double)(max_level - min_level)));
}