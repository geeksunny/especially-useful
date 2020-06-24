#ifndef ESPECIALLY_USEFUL_INCLUDE_LED_H_
#define ESPECIALLY_USEFUL_INCLUDE_LED_H_

#include <cstdint>

namespace led {

enum class LEDAction {
  NONE,
  ON,
  OFF,
  BLINK
};

struct LEDSettings {
  unsigned long timeOn = 0;
  unsigned long timeOff = 0;
  led::LEDAction action = led::LEDAction::NONE;
};

// TODO: Refactor to more of a builder pattern?
class LED {
 public:
  explicit LED(uint8_t pinNumber);
  virtual ~LED();
  static void loop();
  LED &setup();
  void on(unsigned long durationOn = 0, unsigned long delay = 0, bool restoreAfter = false);
  void off(unsigned long durationOff = 0, unsigned long delay = 0, bool restoreAfter = false);
  void toggle(unsigned long duration = 0, unsigned long delay = 0, bool restoreAfter = false);
  void blink(unsigned long durationOn, unsigned long durationOff = 0, unsigned long delay = 0);
  void blinkInverted(unsigned long durationOn, unsigned long durationOff = 0, unsigned long delay = 0);
  void stop(bool cancelStoredAction = false);
  bool isRunning();
  bool isOn();
 private:
  const uint8_t pin_;
  bool delay_ = false;
  unsigned long nextTick_ = 0;

  LEDSettings settings_main_;
  LEDSettings settings_temp_;
  LEDSettings *settings_ = &settings_main_;

  void blink(bool startTurnedOn, unsigned long durationOn, unsigned long durationOff, unsigned long delay);
  void set(bool turnedOn, unsigned long duration = 0, unsigned long delay = 0, bool restoreAfter = false);
  void handleDelay();
  void handleAction();
  void turnOn();
  void turnOff();
  void setTurnedOn(bool turnedOn);
  bool restoreIfNeeded(bool restoreNextTick);
};

}

#endif //ESPECIALLY_USEFUL_INCLUDE_LED_H_
