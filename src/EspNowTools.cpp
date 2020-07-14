#include "EspNowTools.h"
#include "DebugLog.hpp"

#include <ESP8266WiFi.h>
#include <espnow.h>

namespace esp_now_tools {

////////////////////////////////////////////////////////////////
// Class : EspNow //////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/* static */
esp_now_tools::EspNow *esp_now_tools::EspNow::instance;

/* static */
EspNow *EspNow::getInstance() {
  if (!instance) {
    instance = new EspNow();
  }
  return instance;
}

/* static */
EspNow::Builder EspNow::configure(EspNow::Role role) {
  if (instance) {
    // TODO: Should this be moved to the top of Builder::commit() ?
    esp_now_tools::EspNow::reset();
  }
  return Builder(role);
}

/* static */
bool EspNow::unpairAll() {
  uint8_t *mac_addr = esp_now_fetch_peer(true);
  while (mac_addr) {
    esp_now_del_peer(mac_addr);
    mac_addr = esp_now_fetch_peer(false);
    // TODO: is `_del_peer(mac_addr)` going to throw off the result of `_fetch_peer(false)`?
    //  May need to pass true each time....
  }
  return false;
}

bool EspNow::pair(uint8_t *mac_addr, uint8_t channel, esp_now_self_repair_cb_t self_repair_cb) {
  if (esp_now_add_peer(mac_addr, ESP_NOW_ROLE_SLAVE, channel, nullptr, 0) != 0) {
    DEBUG("Error pairing with ESP-NOW server.")
    // Self-repair mode, only if a callback was passed.
    if (self_repair_cb) {
      DEBUG("Starting ESP-NOW channel self-repair procedure");
      int peer_channel = esp_now_get_peer_channel(mac_addr);
      if (peer_channel > 0 && peer_channel < 15) {
        if (wifi_set_channel(peer_channel) && esp_now_set_peer_channel(mac_addr, peer_channel) == 0) {
          // TODO: Do we need to make another call to esp_now_add_peer() here?
          self_repair_cb(peer_channel);
          return true;
        }
      }
    }
    return false;
  }
  return true;
}

bool EspNow::unpair(uint8_t *mac_addr) {
  return esp_now_del_peer(mac_addr) == 0;
}

bool EspNow::send(uint8_t *dest_mac_addr, uint8_t *data, size_t length) {
  return esp_now_send(dest_mac_addr, data, length);
}

EspNow::operator bool() {
  return configured_;
}

EspNow::operator bool() const {
  return configured_;
}

/* static */
EspNow *EspNow::getInstance(bool config_success) {
  if (!instance) {
    instance = new EspNow();
  }
  instance->configured_ = config_success;
  return instance;
}

/* static */
bool EspNow::reset() {
  // Unpairing devices here is unnecessary; They are unpaired during deinit.
  if (esp_now_unregister_send_cb() != 0) {
    // todo: handle failure?
  }
  if (esp_now_unregister_recv_cb() != 0) {
    // todo: handle failure?
  }
  if (esp_now_deinit() != 0) {
    // todo: handle failure?
  }
  getInstance()->configured_ = false;
  return true;
}

////////////////////////////////////////////////////////////////
// Class : EspNow::Builder /////////////////////////////////////
////////////////////////////////////////////////////////////////

EspNow::Builder &EspNow::Builder::setOnSendCallback(esp_now_send_cb_t on_send_cb) {
  on_send_cb_ = on_send_cb;
  return *this;
}

EspNow::Builder &EspNow::Builder::setOnRecvCallback(esp_now_recv_cb_t on_recv_cb) {
  on_recv_cb_ = on_recv_cb;
  return *this;
}

EspNow::Builder &EspNow::Builder::setWifiMode(wifi_mode wifi_mode) {
  wifi_mode_ = wifi_mode;
  return *this;
}

EspNow *EspNow::Builder::commit() {
  // Wifi Mode
  if (wifi_mode_ != wifi_mode::DONT_TOUCH) {
    WiFi.mode((WiFiMode_t) wifi_mode_);
  }
  // ESP-NOW init
  if (esp_now_init() != 0) {
    DEBUG("Error initializing ESP-NOW")
    return EspNow::getInstance(false);
  }
  // ESP-NOW role
  if (esp_now_set_self_role((uint8_t) role_) != 0) {
    DEBUG("Error setting ESP-NOW role.")
    return EspNow::getInstance(false);
  }
  // ESP-NOW onSend callback
  if (on_send_cb_) {
    if (esp_now_register_send_cb(on_send_cb_) != 0) {
      DEBUG("Error registering ESP-NOW OnSend callback.")
      return EspNow::getInstance(false);
    }
  }
  // ESP-NOW onRecv callback
  if (on_recv_cb_) {
    if (esp_now_register_recv_cb(on_recv_cb_) != 0) {
      DEBUG("Error registering ESP-NOW OnRecv callback.")
      return EspNow::getInstance(false);
    }
  }
  return EspNow::getInstance(true);
}

}
