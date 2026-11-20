#ifndef SENSORS_DATA_H
#define SENSORS_DATA_H

#include <inttypes.h>

typedef struct {
  float temperature;
  int32_t pressure;
  float height;
  // GPS todo
} sensor_data_t;

void init_sensors_data();
void get_temperature(float *temp);
void get_height(float *height);
void get_pressure(int32_t *press);
void get_sensor_data(sensor_data_t *data);

void set_temperature(float temp);
void set_height(float height);
void set_pressure(int32_t press);
void set_sensor_data(sensor_data_t data);

#endif
