#ifndef _SSI_SERVER_H_
#define _SSI_SERVER_H_

#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"

// Settings storage in flash offset
#define SETTINGS_FLASH_OFFSET (1536 * 1024)

// Initialize SSI &GCI server parameters
void server_init();

#endif  // _SSI_SERVER_H_