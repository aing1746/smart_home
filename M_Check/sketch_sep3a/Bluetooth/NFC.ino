#include "nfc.h"
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

static uint8_t _sg90Pin = 6;
static uint8_t _led1Pin = 4;
static uint8_t _led2Pin = 3;
static uint8_t _ssPin = 10;
static uint8_t _rstPin = 9;

static Servo SG90;
static MFRC522* rc522 = nullptr;

void nfc_init(uint8_t sg90Pin, uint8_t led1Pin, uint8_t led2Pin, uint8_t ssPin, uint8_t rstPin) {
  _sg90Pin = sg90Pin;
  _led1Pin = led1Pin;
  _led2Pin = led2Pin;
  _ssPin = ssPin;
  _rstPin = rstPin;

  Serial.println("NFC init");
  SPI.begin();
  if (rc522) { delete rc522; rc522 = nullptr; }
  rc522 = new MFRC522(_ssPin, _rstPin);
  rc522->PCD_Init();

  SG90.attach(_sg90Pin);
  pinMode(_led1Pin, OUTPUT);
  pinMode(_led2Pin, OUTPUT);
}

void nfc_update() {
  if (!rc522) return;

  digitalWrite(_led1Pin, LOW);
  digitalWrite(_led2Pin, LOW);

  if ( !rc522->PICC_IsNewCardPresent() || !rc522->PICC_ReadCardSerial() ) {
    delay(200);
    return;
  }

  Serial.print("Card UID:");
  for (byte i = 0; i < 4; i++) {
    Serial.print(rc522->uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // 등록된 UID 예시: B6 F7 18 F8
  if (rc522->uid.uidByte[0]==0xB6 && rc522->uid.uidByte[1]==0xF7 &&
      rc522->uid.uidByte[2]==0x18 && rc522->uid.uidByte[3]==0xF8) {
    Serial.println("<< OK !!! >>  Registered card...");
    digitalWrite(_led1Pin, HIGH);

    for (int i = 0; i <= 180; i++) {
      SG90.write(i);
      delay(10);
    }
    for (int i = 180; i > 0; i--) {
      SG90.write(i);
      delay(10);
    }
    delay(300);
  } else {
    digitalWrite(_led2Pin, HIGH);
    Serial.println("<< WARNING !!! >>  This card is not registered");
    delay(500);
  }
}

//라이브러리 설치 필요함(RFID)