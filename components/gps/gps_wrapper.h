#ifndef GPS_WRAPPER_H
#define GPS_WRAPPER_H

#include "esp_check.h"

esp_err_t gps_init();
void print_raw_data();
#endif