#pragma once

#include <SPI.h>
#include <MFRC522.h>

// 핀 정의 (필요시 변경)
constexpr int RST_PIN_NFC = 5;
constexpr int SS_PIN_NFC  = 10;
constexpr int LED_OK_NFC  = 4;
constexpr int LED_NG_NFC  = 3;

// 동작 각도 (스케치와 일치시킬 것)
constexpr int OPEN_ANGLE_NFC  = 30;
constexpr int CLOSE_ANGLE_NFC = 100;

// 외부 함수(스케치에 구현)
extern void moveServoSmooth(float target, int steps = 50, int stepDelayMs = 10);

// RFID 객체 (헤더에 static으로 선언)
static MFRC522 rc522(SS_PIN_NFC, RST_PIN_NFC);

// 허용 UID (자신 카드로 변경)
static const byte authorizedUID_NFC[4] = { 0xB6, 0xF7, 0x18, 0xF8 };

inline void initNFC() {
  SPI.begin();
  rc522.PCD_Init();
  pinMode(LED_OK_NFC, OUTPUT);
  pinMode(LED_NG_NFC, OUTPUT);
  digitalWrite(LED_OK_NFC, LOW);
  digitalWrite(LED_NG_NFC, LOW);
}

inline void pollNFC() {
  if (!rc522.PICC_IsNewCardPresent()) {
    delay(20);
    return;
  }
  if (!rc522.PICC_ReadCardSerial()) {
    delay(20);
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < rc522.uid.size; i++) {
    Serial.print(rc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  bool ok = (rc522.uid.size >= 4);
  if (ok) {
    for (byte i = 0; i < 4; ++i) {
      if (rc522.uid.uidByte[i] != authorizedUID_NFC[i]) { ok = false; break; }
    }
  }

  if (ok) {
    Serial.println("Authorized - opening");
    digitalWrite(LED_OK_NFC, HIGH);
    digitalWrite(LED_NG_NFC, LOW);
    moveServoSmooth(OPEN_ANGLE_NFC);
    delay(1500);
    moveServoSmooth(CLOSE_ANGLE_NFC);
    digitalWrite(LED_OK_NFC, LOW);
  } else {
    Serial.println("Unauthorized");
    digitalWrite(LED_OK_NFC, LOW);
    digitalWrite(LED_NG_NFC, HIGH);
    delay(800);
    digitalWrite(LED_NG_NFC, LOW);
  }

  rc522.PICC_HaltA();
  rc522.PCD_StopCrypto1();
  delay(200);
}
