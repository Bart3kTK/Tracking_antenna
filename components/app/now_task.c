#include "now_task.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>

#include "now.h"
#include "sensors_data.h"

static const char *TAG = "NOW_TASK";

static void now_send_task(void *arg) {
  (void)arg;
  sensor_data_t data;

  while (1) {
    get_sensor_data(&data);
    esp_err_t err = espnow_send(&data, sizeof(data));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "espnow_send failed: %s", esp_err_to_name(err));
    } else {
      ESP_LOGI(TAG, "sent T=%.2f H=%.2f P=%" PRId32, data.temperature,
               data.height, data.pressure);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

esp_err_t start_now_send_task(void) {
  BaseType_t ok = xTaskCreatePinnedToCore(now_send_task, "now_send_task", 4096,
                                          NULL, 5, NULL, 0);
  return (ok == pdPASS) ? ESP_OK : ESP_FAIL;
}
