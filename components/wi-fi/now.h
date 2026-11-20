#ifndef NOW_H
#define NOW_H

#include "esp_err.h"
#include "esp_now.h"
#include <stddef.h>
#include <stdint.h>

extern const uint8_t ESPNOW_PEER_MAC[ESP_NOW_ETH_ALEN];

void init_wifi_and_espnow(void);
esp_err_t espnow_send(const void *data, size_t len);

#endif
