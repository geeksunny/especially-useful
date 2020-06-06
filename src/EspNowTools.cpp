#include "EspNowTools.h"
#include "DebugLog.hpp"

#include <ESP8266WiFi.h>
#include <espnow.h>

namespace esp_now_tools {

bool startClient(uint8_t *server_mac_addr, esp_now_send_cb_t on_send_cb) {
  // Init wifi in station mode.
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW.
  if (esp_now_init() != 0) {
    DEBUG("Error initializing ESP-NOW")
    return false;
  }
  // Set up role & callback.
  if (esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER) != 0) {
    DEBUG("Error setting ESP-NOW role.")
    return false;
  }
  if (esp_now_register_send_cb(on_send_cb) != 0) {
    DEBUG("Error registering ESP-NOW OnSend callback.")
    return false;
  }
  // Pair with the server address.
  if (esp_now_add_peer(server_mac_addr, ESP_NOW_ROLE_SLAVE, 1, nullptr, 0) != 0) {
    DEBUG("Error pairing with ESP-NOW server.")
    return false;
  }
  return true;
}

bool startServer(esp_now_recv_cb_t on_recv_cb) {
  // Init wifi in station mode.
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW.
  if (esp_now_init() != 0) {
    DEBUG("Error initializing ESP-NOW")
    return false;
  }
  // Set up role & callback.
  if (esp_now_set_self_role(ESP_NOW_ROLE_SLAVE) != 0) {
    DEBUG("Error setting ESP-NOW role.")
    return false;
  }
  if (esp_now_register_recv_cb(on_recv_cb) != 0) {
    DEBUG("Error registering ESP-NOW OnRecv callback.")
    return false;
  }
  return true;
}

int send(uint8_t *dest_mac_addr, uint8_t *data, size_t length) {
  return esp_now_send(dest_mac_addr, data, length);
}

}
