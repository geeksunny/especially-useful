#ifndef ESPECIALLY_USEFUL_INCLUDE_WIFITOOLS_H_
#define ESPECIALLY_USEFUL_INCLUDE_WIFITOOLS_H_

typedef void (*on_wait_cb_t)();

namespace wifi_tools {

bool disableWifiConnection();
bool setMacAddrClient(uint8_t *new_mac_addr);
bool setMacAddrAP(uint8_t *new_mac_addr);
String apName();
bool startAP();
bool startClient(on_wait_cb_t on_wait_cb = nullptr, unsigned long on_wait_freq_ms = 500);

}

#endif //ESPECIALLY_USEFUL_INCLUDE_WIFITOOLS_H_
