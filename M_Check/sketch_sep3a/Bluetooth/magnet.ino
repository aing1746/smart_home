#include "magnet.h"

static uint8_t _reedPin = 3;
static uint8_t _ledPin = 2;

void magnet_init(uint8_t reedPin, uint8_t ledPin) {
  _reedPin = reedPin;
  _ledPin = ledPin;
  pinMode(_reedPin, INPUT);
  pinMode(_ledPin, OUTPUT);
}

void magnet_update() {
  // 리드스위치가 HIGH 면 LED OFF, LOW면 LED ON
  if (digitalRead(_reedPin) == HIGH) {
    digitalWrite(_ledPin, LOW);
  } else {
    digitalWrite(_ledPin, HIGH);
  }
}