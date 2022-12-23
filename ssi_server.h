#ifndef _SSI_SERVER_H_
#define _SSI_SERVER_H_

#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "VL53L1X_pico_api/VL53L1X_types.h"
#include "VL53L1X_pico_api/VL53L1X_api.h"

#include "lwipopts.h"

// max length of the tags defaults to be 8 chars
// LWIP_HTTPD_MAX_TAG_NAME_LEN
const char * __not_in_flash("httpd") ssi_example_tags[] = {
  "cL"
};

u16_t __time_critical_func(ssi_handler)(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
    case 0: /* "currentLevel" */
      printed = snprintf(pcInsert, iInsertLen, "%d", 50);
      break;
    case 1: /* "maxLevel" */
      printed = snprintf(pcInsert, iInsertLen, "%d", 50);
      break;
    default: /* unknown tag */
      printed = 0;
      break;
  }
  LWIP_ASSERT("sane length", printed <= 0xFFFF);
  return (u16_t)printed;
}

void ssi_init() {
  size_t i;
  for (i = 0; i < LWIP_ARRAYSIZE(ssi_example_tags); i++) {
    LWIP_ASSERT("tag too long for LWIP_HTTPD_MAX_TAG_NAME_LEN",
      strlen(ssi_example_tags[i]) <= LWIP_HTTPD_MAX_TAG_NAME_LEN);
  }

  http_set_ssi_handler(ssi_handler,
    ssi_example_tags, LWIP_ARRAYSIZE(ssi_example_tags)
    );
}

#endif  // _SSI_SERVER_H_