#ifndef ESPECIALLY_USEFUL_INCLUDE_ESPNOWTOOLS_H_
#define ESPECIALLY_USEFUL_INCLUDE_ESPNOWTOOLS_H_

#include <cstdint>
#include <cstddef>

#define ESP_NOW_GET_DATA(dest, src) memcpy(&dest, src, sizeof(dest));

namespace esp_now_tools {

// These definitions were pulled from esp8266 ESP-NOW / Wifi library to avoid build issues with includes.
//  Names and values have been tweaked to avoid name clashes.
typedef void (*esp_now_onrecv_cb_t)(uint8_t *mac_addr, uint8_t *data, uint8_t len);
typedef void (*esp_now_onsend_cb_t)(uint8_t *mac_addr, uint8_t status);
enum class wifi_mode {
  // Prevent any changes to wifi during setup
  DONT_TOUCH,
  // Values should align with ESP8266WiFiType.h::WiFiMode
  STA = 1,
  AP,
  AP_STA
};

typedef void (*esp_now_self_repair_cb_t)(uint8_t new_channel);

// TODO: Build-in the onrecv callback as simple function to copy data to local `result` member variable;
//  Implementor's onrecv callback would use actual result type as parameter;
//  Callback would be triggered by a static call to `EspNow::loop();` added to main loop() function.
//   Checks for value set to `result`, calls callback if found
//   This approach should avoid any processing on the interrupt
class EspNow {
  friend class Builder;
  static EspNow *instance;

 public:
  enum class Role {
    // Values should align with espnow.h::esp_now_role
    CONTROLLER = 1,
    SLAVE,
    COMBO,
    MAX,
    // Alias values
    CLIENT = 1,
    SERVER
  };

  class Builder {
    friend class EspNow;
   public:
    Builder &setOnSendCallback(esp_now_onsend_cb_t on_send_cb);
    Builder &setOnRecvCallback(esp_now_onrecv_cb_t on_recv_cb);
    Builder &setWifiMode(wifi_mode wifi_mode);
    EspNow *commit();
   private:
    Role role_;
    esp_now_onsend_cb_t on_send_cb_ = nullptr;
    esp_now_onrecv_cb_t on_recv_cb_ = nullptr;
    wifi_mode wifi_mode_ = wifi_mode::STA;

    explicit Builder(Role role) : role_(role) {}
  };

  static EspNow* getInstance();
  static EspNow::Builder configure(Role role);
  static bool unpairAll();

  // Self-repair mode only enabled if a `self_repair_cb` is defined.
  bool pair(uint8_t *mac_addr, uint8_t channel = 0, esp_now_self_repair_cb_t self_repair_cb = nullptr);
  bool unpair(uint8_t *mac_addr);
  bool send(uint8_t *dest_mac_addr, uint8_t *data, size_t length);

  template<typename T>
  bool send(uint8_t *dest_mac_addr, T data) {
    return send(dest_mac_addr, (uint8_t *) &data, sizeof(T));
  }
  template<typename T>
  bool sendToAllPeers(T data) {
    // TODO: Does this work if peers are on different channels from one another? Will wifi auto-switch?
    return send(nullptr, (uint8_t *) &data, sizeof(T));
  }

  explicit operator bool();
  explicit operator bool() const;

 private:
  static EspNow* getInstance(bool config_success);
  static bool reset();

  bool configured_ = false;

  EspNow() = default;
};

}

#endif //ESPECIALLY_USEFUL_INCLUDE_ESPNOWTOOLS_H_
