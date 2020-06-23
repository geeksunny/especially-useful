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
    // TODO: Should there be a flag check to ensure ::setup() was called?
    if (led->action_ == LEDAction::NONE) {
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

void LED::on(const unsigned long durationOn, const unsigned long delay) {
  set(true, durationOn, delay);
}

void LED::off(const unsigned long durationOff, const unsigned long delay) {
  set(false, durationOff, delay);
}

void LED::toggle(const unsigned long duration, const unsigned long delay) {
  set((digitalRead(pin_) == LOW), duration, delay);
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
    timeOn_ = durationOn;
    timeOff_ = (durationOff == 0) ? durationOn : durationOff;
    if (delay > 0) {
      delay_ = true;
      nextTick_ = millis() + delay;
    } else {
      nextTick_ = millis() + (startTurnedOn ? timeOn_ : timeOff_);
    }
    action_ = LEDAction::BLINK;
  }
}

void LED::set(const bool turnedOn, const unsigned long duration, const unsigned long delay) {
  DEBUG("LED::set( ", turnedOn, " , ", duration, " , ", delay, " )")
  if (duration == 0 && delay == 0) {
    action_ = LEDAction::NONE;
    digitalWrite(pin_, (turnedOn ? HIGH : LOW));
    return;
  } else if (delay > 0) {
    delay_ = true;
    nextTick_ = millis() + delay;
    if (turnedOn) {
      timeOn_ = duration;
    } else {
      timeOff_ = duration;
    }
  } else {
    delay_ = false;
    nextTick_ = millis() + duration;
  }
  action_ = turnedOn ? LEDAction::ON : LEDAction::OFF;
}

void LED::stop() {
  action_ = LEDAction::NONE;
}

bool LED::isRunning() {
  return action_ != LEDAction::NONE;
}

bool LED::isOn() {
  return digitalRead(pin_) == HIGH;
}

void LED::handleDelay() {
  delay_ = false;
  switch (action_) {
    case LEDAction::ON: {
      turnOn();
      if (timeOn_ > 0) {
        nextTick_ = millis() + timeOn_;
      }
      break;
    }
    case LEDAction::OFF: {
      turnOff();
      if (timeOff_ > 0) {
        nextTick_ = millis() + timeOff_;
      }
      break;
    }
    case LEDAction::BLINK: {
      bool setTo = !isOn();
      setTurnedOn(setTo);
      nextTick_ = millis() + ((setTo) ? timeOn_ : timeOff_);
      break;
    }
    case LEDAction::NONE: {
      // Shouldn't be here
    }
  }
}

void LED::handleAction() {
  switch (action_) {
    case LEDAction::ON: {
      turnOn();
      stop();
      break;
    }
    case LEDAction::OFF: {
      turnOff();
      stop();
      break;
    }
    case LEDAction::BLINK: {
      bool litUp = isOn();
      nextTick_ = millis() + (litUp ? timeOff_ : timeOn_);
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

}
