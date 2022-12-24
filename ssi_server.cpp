#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "pico/util/queue.h"

#include "lwipopts.h"

#include "ssi_server.h"

extern queue_t raw_level; // Extern variable from main

// max length of the tags defaults to be 8 chars
// LWIP_HTTPD_MAX_TAG_NAME_LEN
const char * __not_in_flash("httpd") ssi_example_tags[] = {
  "currL", "minL", "maxL", 
};

// Use __time_critical_func to run the function from RAM
u16_t __time_critical_func(ssi_handler)(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
    case 0:  // current level
      int curr_level;
      queue_peek_blocking(&raw_level, &curr_level);
      printed = snprintf(pcInsert, iInsertLen, "%d", curr_level);
      break;
    case 1:  // min level
      printed = snprintf(pcInsert, iInsertLen, "%d", 89);
      break;
    case 2:  // max level

    default: // unknown
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