#ifndef MS5611_H
#define MS5611_H

#include <esp_log.h>
#include <ms5611.h>

typedef struct {
  int32_t pressure;
  float temperature;

} ms5611_data_t;

esp_err_t ms5611_config_init();
esp_err_t ms5611_get_data(ms5611_data_t *data);

#endif