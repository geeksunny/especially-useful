#ifdef USEFUL_EXAMPLE_PROJECT
#include <Arduino.h>
#include "DebugLog.hpp"
#include "WifiTools.h"
#include "LED.h"

#ifndef IS_DEBUG_MODE
#define IS_DEBUG_MODE
#endif

// LED Pin configuration
#include <pins_arduino.h>
#ifndef PIN_LED_PRIMARY
#define PIN_LED_PRIMARY       D6
#endif
#ifndef PIN_LED_SECONDARY
#define PIN_LED_SECONDARY     D7
#endif
// END LED Pin configuration

#define BLINK_DURATION_QUICK      250
#define BLINK_DURATION_STANDARD   500
#define BLINK_DURATION_LONG       1000

#define DEMO_SHIFT_DURATION       5000

enum led_demo_mode {
  BLINK_QUICK,
  BLINK_STANDARD,
  BLINK_LONG,
  ON_OFF,
  OFF_ON,
  RESET
};

unsigned long now;
unsigned long nextTick = 0;
short demo_mode = RESET;
led::LED *ledPrimary;
led::LED *ledSecondary;

void on_wait_wifi_cb() {
  DEBUG("Wifi not yet available...")
  led::LED::loop();
}

void setup() {
  SETUP_SERIAL(BAUD_RATE, 3000, "Serial console ready.")
  // Set up LEDs to blink during wifi connection
  ledPrimary = new led::LED(PIN_LED_PRIMARY);
  ledSecondary = new led::LED(PIN_LED_SECONDARY);
  ledPrimary->setup();
  ledSecondary->setup();
  // Connect to wifi with a custom wait status callback
  wifi_tools::startClient(on_wait_wifi_cb, BLINK_DURATION_QUICK);
}

void set_led_demo_mode(enum led_demo_mode &mode) {
  if (mode >= RESET) {
    mode = BLINK_QUICK;
  }
  switch (mode) {
    case BLINK_QUICK:
      DEBUG("::BLINK_QUICK::")
      ledPrimary->blink(BLINK_DURATION_QUICK);
      ledSecondary->blinkInverted(BLINK_DURATION_QUICK);
      break;
    case BLINK_STANDARD:
      DEBUG("::BLINK_STANDARD::")
      ledPrimary->blink(BLINK_DURATION_STANDARD);
      ledSecondary->blinkInverted(BLINK_DURATION_STANDARD);
      break;
    case BLINK_LONG:
      DEBUG("::BLINK_LONG::")
      ledPrimary->blink(BLINK_DURATION_LONG);
      ledSecondary->blinkInverted(BLINK_DURATION_LONG);
      break;
    case ON_OFF:
      DEBUG("::LONG-ON_OFF::restoreAfter")
      ledPrimary->on(BLINK_DURATION_LONG, 0, true);
      ledSecondary->off(BLINK_DURATION_LONG, 0, true);
      break;
    case OFF_ON:
      DEBUG("::OFF_ON::")
      ledPrimary->off();
      ledSecondary->on();
      break;
    default:
      break;
  }
}

void loop() {
  now = millis();
  if (now >= nextTick) {
    DEBUG("CHANGING LED DEMO MODE")
    set_led_demo_mode((led_demo_mode&) ++demo_mode);
    nextTick = now + DEMO_SHIFT_DURATION;
  }
  led::LED::loop();
}
#endif //USEFUL_EXAMPLE_PROJECT