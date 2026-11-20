#include "gps_wrapper.h"
#include "neo6m.h"

esp_err_t gps_init() {
  gps_start();
  return ESP_OK;
}

void print_raw_data() { raw_nmea(); }