#pragma once
#include <Arduino.h>

void nfc_init(uint8_t sg90Pin, uint8_t led1Pin, uint8_t led2Pin, uint8_t ssPin, uint8_t rstPin);
void nfc_update();