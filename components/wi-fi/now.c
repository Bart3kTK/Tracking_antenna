#include "now.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sensors_data.h"
#include <inttypes.h>
#include <string.h>

static const char *TAG = "ESP_NOW";

const uint8_t ESPNOW_PEER_MAC[ESP_NOW_ETH_ALEN] = {0x11, 0x11, 0x11,
                                                   0x11, 0x11, 0x11};

static void on_data_sent(const uint8_t *mac_addr,
                         esp_now_send_status_t status) {
  (void)mac_addr;
  ESP_LOGI(TAG, "Send status: %s",
           status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

static void on_data_recv(const esp_now_recv_info_t *recv_info,
                         const uint8_t *data, int len) {
  if (recv_info == NULL || data == NULL) {
    return;
  }

  if (len == (int)sizeof(sensor_data_t)) {
    sensor_data_t payload;
    memcpy(&payload, data, sizeof(payload));
    ESP_LOGI(TAG, "Recv from " MACSTR " T=%.2f H=%.2f P=%" PRId32,
             MAC2STR(recv_info->src_addr), payload.temperature, payload.height,
             payload.pressure);
    return;
  }

  ESP_LOGI(TAG, "Recv from " MACSTR " (%d bytes)", MAC2STR(recv_info->src_addr),
           len);
}

esp_err_t espnow_send(const void *data, size_t len) {
  if (data == NULL || len == 0) {
    return ESP_ERR_INVALID_ARG;
  }
  return esp_now_send(ESPNOW_PEER_MAC, (const uint8_t *)data, len);
}

void init_wifi_and_espnow(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

  uint8_t custom_mac[6] = {0x5A, 0x31, 0xE7, 0xAA, 0x9A, 0xAA};
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, custom_mac));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(on_data_sent));
  ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));

  esp_now_peer_info_t peer = {
      .channel = 1,
      .ifidx = WIFI_IF_STA,
      .encrypt = false,
  };
  memcpy(peer.peer_addr, ESPNOW_PEER_MAC, ESP_NOW_ETH_ALEN);
  ESP_ERROR_CHECK(esp_now_add_peer(&peer));
}
