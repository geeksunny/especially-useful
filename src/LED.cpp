#include <Arduino.h>
#include <deque>
#include "DebugLog.hpp"
#include "LED.h"

namespace led {

std::deque<LED *> leds;

LED::LED(const uint8_t pinNumber) : pin_(pinNumber) {
  leds.push_back(this);
}

LED::~LED() {
  for (auto it = leds.begin(); it != leds.end(); ++it) {
    if (*it == this) {
      leds.erase(it);
      return;
    }
  }
}

void LED::loop() {
  if (leds.empty()) {
    return;
  }
  unsigned long now = millis();
  for (auto &led : leds) {
    if (led->settings_->action == LEDAction::NONE) {
      continue;
    }
    if (now >= led->nextTick_) {
      if (led->delay_) {
        led->handleDelay();
      } else {
        led->handleAction();
      }
    }
  }
}

LED &LED::setup() {
  pinMode(pin_, OUTPUT);
  return *this;
}

void LED::on(const unsigned long durationOn, const unsigned long delay, const bool restoreAfter) {
  set(true, durationOn, delay, restoreAfter);
}

void LED::off(const unsigned long durationOff, const unsigned long delay, const bool restoreAfter) {
  set(false, durationOff, delay, restoreAfter);
}

void LED::toggle(const unsigned long duration, const unsigned long delay, const bool restoreAfter) {
  set((digitalRead(pin_) == LOW), duration, delay, restoreAfter);
}

void LED::blink(const unsigned long durationOn, const unsigned long durationOff, const unsigned long delay) {
  blink(true, durationOn, durationOff, delay);
}

void LED::blinkInverted(const unsigned long durationOn, const unsigned long durationOff, const unsigned long delay) {
  blink(false, durationOn, durationOff, delay);
}

void LED::blink(const bool startTurnedOn,
                const unsigned long durationOn,
                const unsigned long durationOff,
                const unsigned long delay) {
  if (delay == 0) {
    setTurnedOn(startTurnedOn);
  }
  if (durationOn > 0) {
    settings_->timeOn = durationOn;
    settings_->timeOff = (durationOff == 0) ? durationOn : durationOff;
    if (delay > 0) {
      delay_ = true;
      nextTick_ = millis() + delay;
    } else {
      nextTick_ = millis() + (startTurnedOn ? settings_->timeOn : settings_->timeOff);
    }
    settings_->action = LEDAction::BLINK;
  }
}

void LED::set(const bool turnedOn, const unsigned long duration, const unsigned long delay, const bool restoreAfter) {
  DEBUG("LED::set( ", turnedOn, " , ", duration, " , ", delay, " , ", restoreAfter, " )")
  // If immediate and continual, write to the pin and set action to NONE.
  if (duration == 0 && delay == 0) {
    settings_->action = LEDAction::NONE;
    digitalWrite(pin_, (turnedOn ? HIGH : LOW));
    return;
  }
  // Change to alternate settings object if restoreAfter flag is set to `true.`
  if (restoreAfter) {
    settings_ = &settings_temp_;
  }
  if (delay > 0) {
    delay_ = true;
    nextTick_ = millis() + delay;
    if (turnedOn) {
      settings_->timeOn = duration;
    } else {
      settings_->timeOff = duration;
    }
  } else {
    delay_ = false;
    nextTick_ = millis() + duration;
  }
  settings_->action = turnedOn ? LEDAction::ON : LEDAction::OFF;
}

void LED::stop(const bool cancelStoredAction) {
  if (!restoreIfNeeded(!cancelStoredAction) || cancelStoredAction) {
    // Set current action to NONE if a restore is not needed
    //  OR if cancelStoredAction is `true.`
    settings_->action = LEDAction::NONE;
  }
}

bool LED::isRunning() {
  return settings_->action != LEDAction::NONE;
}

bool LED::isOn() {
  return digitalRead(pin_) == HIGH;
}

void LED::handleDelay() {
  delay_ = false;
  switch (settings_->action) {
    case LEDAction::ON: {
      turnOn();
      if (settings_->timeOn > 0) {
        nextTick_ = millis() + settings_->timeOn;
      } else {
        stop();
      }
      break;
    }
    case LEDAction::OFF: {
      turnOff();
      if (settings_->timeOff > 0) {
        nextTick_ = millis() + settings_->timeOff;
      } else {
        stop();
      }
      break;
    }
    case LEDAction::BLINK: {
      bool setTo = !isOn();
      setTurnedOn(setTo);
      nextTick_ = millis() + ((setTo) ? settings_->timeOn : settings_->timeOff);
      break;
    }
    case LEDAction::NONE: {
      // Shouldn't be here
    }
  }
}

void LED::handleAction() {
  switch (settings_->action) {
    case LEDAction::ON: {
      turnOff();
      stop();
      break;
    }
    case LEDAction::OFF: {
      turnOn();
      stop();
      break;
    }
    case LEDAction::BLINK: {
      bool litUp = isOn();
      nextTick_ = millis() + (litUp ? settings_->timeOff : settings_->timeOn);
      setTurnedOn(!litUp);
      break;
    }
    case LEDAction::NONE: {
      // Shouldn't be here
    }
  }
}

void LED::turnOn() {
  DEBUG("LED::turnOn");
  digitalWrite(pin_, HIGH);
}

void LED::turnOff() {
  DEBUG("LED::turnOff");
  digitalWrite(pin_, LOW);
}

void LED::setTurnedOn(bool turnedOn) {
  DEBUG("LED::setTurnedOn", (turnedOn ? "TRUE" : "FALSE"));
  digitalWrite(pin_, (turnedOn ? HIGH : LOW));
}

bool LED::restoreIfNeeded(const bool restoreNextTick) {
  if (settings_ == &settings_temp_) {
    settings_ = &settings_main_;
    if (restoreNextTick) {
      switch (settings_->action) {
        case LEDAction::ON:
          nextTick_ = millis() + settings_->timeOn;
          break;
        case LEDAction::OFF:
          nextTick_ = millis() + settings_->timeOff;
          break;
        case LEDAction::BLINK:
          nextTick_ = millis() + (isOn() ? settings_->timeOff : settings_->timeOn);
          break;
        case LEDAction::NONE:break;
          // No action needed
      }
    }
    return true;
  }
  return false;
}

}
