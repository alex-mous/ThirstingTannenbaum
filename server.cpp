#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "pico/util/queue.h"
#include "hardware/flash.h"

#include "lwipopts.h"

#include "server.h"

// Extern variables from main.cpp
extern queue_t raw_level_mm;
extern uint16_t min_level, max_level;

// storage in flash for settings (min/max level)
// Set the board settings (min, max level) to flash
void write_settings_flash() {
  uint8_t buf[FLASH_PAGE_SIZE];
  memset(buf, 0x0, FLASH_PAGE_SIZE);
  *((uint16_t*)buf) = min_level;
  *(1+(uint16_t*)buf) = max_level;
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(SETTINGS_FLASH_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(SETTINGS_FLASH_OFFSET, buf, FLASH_PAGE_SIZE);
  restore_interrupts(ints);
}

// Store tags for SSI (server side includes)
// max length of the tags defaults to be 8 chars - LWIP_HTTPD_MAX_TAG_NAME_LEN
const char * __not_in_flash("httpd") ssi_tags[] = {
  "currLMm", "minLMm", "maxLMm", 
};

// SSI handler to process SSI requests
// Use __time_critical_func to run the function from RAM
u16_t __time_critical_func(ssi_handler)(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
    case 0:  // current level mm
      uint16_t curr_level_mm;
      queue_peek_blocking(&raw_level_mm, &curr_level_mm);
      printed = snprintf(pcInsert, iInsertLen, "%d", curr_level_mm);
      break;
    case 1:  // min level mm
      printed = snprintf(pcInsert, iInsertLen, "%d", min_level);
      break;
    case 2:  // max level mm
      printed = snprintf(pcInsert, iInsertLen, "%d", max_level);
      break;
    default: // unknown
      printed = 0;
      break;
  }
  LWIP_ASSERT("sane length", printed <= 0xFFFF);
  return (u16_t)printed;
}

// CGI handler to process CGI requests
// As above, time critical
u16_t __time_critical_func(cgi_settings_handler)(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
    case 0:  // current level mm
      uint16_t curr_level_mm;
      queue_peek_blocking(&raw_level_mm, &curr_level_mm);
      printed = snprintf(pcInsert, iInsertLen, "%d", curr_level_mm);
      break;
    case 1:  // min level mm
      printed = snprintf(pcInsert, iInsertLen, "%d", min_level);
      break;
    case 2:  // max level mm
      printed = snprintf(pcInsert, iInsertLen, "%d", max_level);
      break;
    default: // unknown
      printed = 0;
      break;
  }
  LWIP_ASSERT("sane length", printed <= 0xFFFF);
  return (u16_t)printed;
}

// Define cgi handlers
const char* __time_critical_func(cgi_settings_handler)(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  if (iNumParams == 2) {
    if (!strcmp(pcParam[0], "min")) {
      if (!strcmp(pcParam[1], "max")) {
        sscanf(pcValue[0], "%hu", &min_level);
        sscanf(pcValue[1], "%hu", &max_level);
        write_settings_flash();
        return "/reboot.html";
      }
    }
  }
  return "/index.shtml";
}

static const tCGI __not_in_flash("httpd_cgi") cgi_handlers[] = {
  {
    "/updatesettings",
    cgi_settings_handler
  }
};


void server_init() {
  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
  http_set_cgi_handlers(cgi_handlers, LWIP_ARRAYSIZE(cgi_handlers));
}