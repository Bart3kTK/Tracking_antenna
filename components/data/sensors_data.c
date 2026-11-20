#include "sensors_data.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static sensor_data_t sensor_data = {
    .temperature = 0.0, .pressure = 0, .height = 0.0};
static SemaphoreHandle_t data_mutex;

void init_sensors_data() { data_mutex = xSemaphoreCreateMutex(); }

void get_temperature(float *temp) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    *temp = sensor_data.temperature;
    xSemaphoreGive(data_mutex);
  }
}

void get_height(float *height) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    *height = sensor_data.height;
    xSemaphoreGive(data_mutex);
  }
}

void get_pressure(int32_t *press) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    *press = sensor_data.pressure;
    xSemaphoreGive(data_mutex);
  }
}

void get_sensor_data(sensor_data_t *data) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    *data = sensor_data;
    xSemaphoreGive(data_mutex);
  }
}

void set_temperature(float temp) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    sensor_data.temperature = temp;
    xSemaphoreGive(data_mutex);
  }
}

void set_height(float height) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    sensor_data.height = height;
    xSemaphoreGive(data_mutex);
  }
}

void set_pressure(int32_t press) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    sensor_data.pressure = press;
    xSemaphoreGive(data_mutex);
  }
}

void set_sensor_data(sensor_data_t data) {
  if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
    sensor_data = data;
    xSemaphoreGive(data_mutex);
  }
}
