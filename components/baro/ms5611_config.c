#include "ms5611_config.h"
#include "pinout.h"

ms5611_t dev = {0};

esp_err_t ms5611_config_init() {

  esp_err_t res =
      ms5611_init_desc(&dev, MS5611_ADDR_CSB_LOW, 0, SDA_GPIO, SCL_GPIO);
  res |= ms5611_init(&dev, MS5611_OSR_1024);
  return res;
}

esp_err_t ms5611_get_data(ms5611_data_t *data) {
  return ms5611_get_sensor_data(&dev, &data->pressure, &data->temperature);
}
