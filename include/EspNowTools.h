#ifndef ESPECIALLY_USEFUL_INCLUDE_ESPNOWTOOLS_H_
#define ESPECIALLY_USEFUL_INCLUDE_ESPNOWTOOLS_H_

#include <cstdint>
#include <cstddef>

#define ESP_NOW_SEND(dest_mac_addr, data) esp_now_tools::send(dest_mac_addr, (uint8_t *) &data, sizeof(data));

namespace esp_now_tools {

typedef void (*esp_now_recv_cb_t)(uint8_t *mac_addr, uint8_t *data, uint8_t len);
typedef void (*esp_now_send_cb_t)(uint8_t *mac_addr, uint8_t status);

bool startClient(uint8_t *server_mac_addr, esp_now_send_cb_t on_send_cb);
bool startServer(esp_now_recv_cb_t on_recv_cb);

int send(uint8_t *dest_mac_addr, uint8_t *data, size_t length);

}

#endif //ESPECIALLY_USEFUL_INCLUDE_ESPNOWTOOLS_H_
