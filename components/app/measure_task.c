
#include "measure_task.h"
#include "kalman_2d_filter.h"
#include "ms5611_config.h"
#include "sensors_data.h"
#include "system_math.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// static esp_err_t barometer_task;

void barometer_task(void *arg) {
  ms5611_data_t baro_data;
  esp_err_t res = ESP_OK;
  kalman_state_t kalman_struct = {
      .q = 0.001f, .r = 0.05f, .p = 1.0f, .x = 0.0f, .is_initialized = false};

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(500));
    res |= ms5611_get_data(&baro_data);
    if (res != ESP_OK)

      continue;
    float height = height_calc(baro_data.pressure, baro_data.temperature);
    height = filter_kalman_2d(&kalman_struct, height);
    set_height(height);
    set_pressure(baro_data.pressure);
    set_temperature(baro_data.temperature);
    ESP_LOGI("BARO", "Wysokosc %.2f i cisneinie %d", height,
             (int)baro_data.pressure);
  }
}

esp_err_t start_barometer_task() {
  xTaskCreatePinnedToCore(barometer_task, "barometer_task", 4096 * 8, NULL, 5,
                          NULL, 0);
  return ESP_OK;
}
