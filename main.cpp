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
#include "ssi_server.h"
#include "credentials.h"

// I/O
#include "io.h"

// Helpers
#include "status.h"


// Interface constants
#define WIFI_CONNECT_TIMEOUT_MS 5000
#define VL53L1X_ADDR 0x29

// Define min
#define min(a,b) ({ __typeof__ (a) _a = (a);  __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

using std::string;

// raw sensor measurement (1 element queue)
queue_t raw_level; 

// min, max level
uint16_t min_level, max_level;


// Start the webserver. Blocks until connection is established.
void start_server();
// Initialize the VL53L1X sensor
VL53L1X_Status_t measurement_init();
// Continuously measure distance from the VL53L1X sensor
void measurement_core();
// Callback to run filling mode
void filling_mode_cb(uint gpio, uint32_t event_mask);


int main() {
    // stdio/board init
    stdio_init_all();
    if (cyw43_arch_init()) {
        status_fatal("Failed to start CYW43.");
    }

    // Init GPIO devices
    leds_init();
    buzzer_init();
    switch_init(reinterpret_cast<gpio_irq_callback_t>(&filling_mode_cb));

    // Flash LEDs to show successful start
    status_warn("Starting Pico...");
    start_server();

    // Initialize queue
    queue_init(&raw_level, sizeof(uint16_t), 1);

    // Initialize VL53L1X
    measurement_init();

    // Start continuous measurement
    multicore_launch_core1(measurement_core);


    // TODO:
    // --- add buzzer functionality (errors, startup, fillingmode, low??)
    // --- finish led scale for measurement
    // --- write filling mode (signal start with buzzer, etc)
    // --- read/write values to flash for max/min level

    // Initialize the needed hardware
    for (;;) {};
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
            status_log("Connected. Starting server at " + string(ip4addr_ntoa(netif_ip4_addr(netif_list))));
            break;
        }
    }

    // Start the webserver
    httpd_init();
    ssi_init();
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

        std::stringstream ss;
        ss << "Packet received. Status = " << results.status
                        << ", dist = " << results.distance
                        << ", ambient = " << results.ambient
                        << ", signal = " << results.sigPerSPAD
                        << ", #ofSpads = " << results.numSPADs << std::endl;
        status_log(ss.str());

        // Set LEDs based on value
        if (results.distance < max_level)
            max_level = results.distance;
        if (results.distance > min_level)
            min_level = results.distance;
        uint16_t scale_full = (255*(max_level - results.distance)) / (max_level - min_level);
        leds_set(min(2*scale_full, 255), min(2*(255-scale_full), 255), 0);

        // Remove old value (after first run) and add new value
        if (!first_range)
            queue_remove_blocking(&raw_level, nullptr);
        queue_add_blocking(&raw_level, reinterpret_cast<void*>(&scale_full));

        // Clear the sensor for a new measurement
		status += VL53L1X_ClearInterrupt(VL53L1X_ADDR);
		if (first_range) {  // Clear twice on first measurement
			status += VL53L1X_ClearInterrupt(VL53L1X_ADDR);
			first_range = false;
		}
	}
}

void filling_mode_cb(uint gpio, uint32_t event_mask) {
    while (gpio_get(gpio) == 0) {  // Run while switch is enabled
        
    }
}