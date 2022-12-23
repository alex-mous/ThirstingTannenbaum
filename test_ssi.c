#include "lwip/opt.h"
#include "lwip/apps/httpd.h"

#include "lwip/def.h"
#include "lwip/mem.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include <stdio.h>
#include <string.h>

#include "ssi_server.h"

#include "credentials.h"

void run_server() {
    httpd_init();
    ssi_init();
    printf("Http server initialized.\n");
    // infinite loop for now
    for (;;) { printf("running..."); sleep_ms(1000);}
}

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    for (int i=0; i<25; i++) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
        printf("Waiting...\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
    }

    cyw43_arch_enable_sta_mode();

    while (1) {
        printf("Connecting to WiFi...\n");
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PSK, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
            printf("failed to connect.\n");
            sleep_ms(1000);
        } else {
            printf("Connected.\n");
            printf("Starting server at %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));
            break;
        }
    }


    run_server();
}