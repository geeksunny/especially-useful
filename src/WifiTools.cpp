#include <ESP8266WiFi.h>
#include "WifiTools.h"
#include "DebugLog.hpp"

/* Default macro values */
#ifndef AP_NAME
#define AP_NAME "Esp8266_AP"
#endif
#ifndef AP_MAC_IN_NAME
#define AP_MAC_IN_NAME false
#endif
#ifndef AP_ENCRYPTED
#define AP_ENCRYPTED true
#endif
#ifndef AP_PASSWORD
#define AP_PASSWORD "Esp8266_PASSWORD"
#endif
#ifndef CLIENT_SSID
#define CLIENT_SSID "NetworkName"
#endif
#ifndef CLIENT_ENCRYPTED
#define CLIENT_ENCRYPTED true
#endif
#ifndef CLIENT_PASS
#define CLIENT_PASS "NetworkPassword"
#endif
/* End of Default macro values */

#if AP_MAC_IN_NAME
#include <sstream>
#include <iomanip>
#endif

namespace wifi_tools {

bool disableWifiConnection() {
  // TODO: Anything else here need to be considered?
  return WiFi.setAutoConnect(false);
}

bool setMacAddrClient(uint8_t *new_mac_addr) {
//  return WiFi.mode(WIFI_STA) && wifi_set_macaddr(STATION_IF, new_mac_addr);
  if (!WiFi.mode(WIFI_STA)) {
    DEBUG("Error setting Wifi mode to STATION")
    return false;
  }
  if (!wifi_set_macaddr(STATION_IF, new_mac_addr)) {
    DEBUG("Error setting Wifi STATION MAC Address.")
    return false;
  }
  return true;
}

bool setMacAddrAP(uint8_t *new_mac_addr) {
//  return WiFi.mode(WIFI_AP) && wifi_set_macaddr(SOFTAP_IF, new_mac_addr);
  if (!WiFi.mode(WIFI_AP)) {
    DEBUG("Error setting Wifi mode to AP")
    return false;
  }
  if (!wifi_set_macaddr(SOFTAP_IF, new_mac_addr)) {
    DEBUG("Error setting Wifi AP MAC Address.")
    return false;
  }
  return true;
}

String apName() {
#if AP_MAC_IN_NAME
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  std::ostringstream os(AP_NAME);
  os << "_" << std::hex << std::setfill('0') << std::setw(2)
     << mac[0] << mac[1] << mac[2] << mac[3] << mac[4] << mac[5];
  return String{os.str().c_str()};
#else
  return AP_NAME;
#endif
}

bool startAP() {
  String name = apName();

  WiFi.mode(WIFI_AP);
#if AP_ENCRYPTED
  bool started = WiFi.softAP(name, AP_PASSWORD);
#else
  bool started = WiFi.softAP(name);
#endif

#ifdef IS_DEBUG_MODE
  if (started) {
    std::cout << "Started AP with name: " << name.c_str() << " | Encrypted: " << (AP_ENCRYPTED ? "YES" : "NO") << std::endl;
    std::cout << "Server IP: " << WiFi.softAPIP().toString().c_str() << std::endl;
  } else {
    std::cout << "FAILED to start wifi AP!";
  }
#endif

  return started;
}

bool startClient(on_wait_cb_t on_wait_cb, unsigned long on_wait_freq_ms) {
  // Check for presence of wifi shield / module
  if (WiFi.status() == WL_NO_SHIELD) {
    DEBUG("WiFi shield not present")
    while (true); // Halt
    // TODO: Properly handle halt with power down
  }

  // Wifi mode set
  WiFi.mode(WIFI_STA);

  // Attempt to connect to network
  // TODO: Figure out how to check if there is any stored WIFI credentials with .getAutoConnect() check!!
  if (WiFi.getAutoConnect()) {
    DEBUG("Attempting to connect using SAVED SSID!")
    // Connect to saved network:
//    WiFi
//    .begin();
  } else {
#if CLIENT_ENCRYPTED
    DEBUG("Attempting to connect to WPA SSID:", CLIENT_SSID)
    // Connect to WPA/WPA2 network:
    WiFi.begin(CLIENT_SSID, CLIENT_PASS);
#else
    DEBUG("Attempting to connect to OPEN WIFI SSID:", CLIENT_SSID)
    // Connect to unencrypted wifi network:
    WiFi.begin(CLIENT_SSID);
#endif
  }

  while (WiFi.status() != WL_CONNECTED) {
    if (on_wait_cb) {
      on_wait_cb();
    } else {
#ifdef IS_DEBUG_MODE
      std::cout << ".";
#endif
    }
    // wait on_wait_freq_ms for connection:
    delay(on_wait_freq_ms);
    // TODO: Handle a wifi timeout period, return false on timeout
  }

  // Connected to network!
  DEBUG("Connected to network!")
  return true;
}

}
