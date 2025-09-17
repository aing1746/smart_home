// ...existing code...
#pragma once
#include <SPI.h>
#include <MFRC522.h>

// 핀(필요시 수정)
constexpr int RST_PIN = 5;
constexpr int SS_PIN  = 10;
constexpr int LED_OK  = 4;
constexpr int LED_NG  = 3;

// 외부로 정의된 함수(스케치에서 구현)
extern void moveServoSmooth(float target, int steps /*=50*/, int stepDelayMs /*=10*/);

// RFID 객체 (static으로 헤더에 넣어도 됨)
static MFRC522 rc522(SS_PIN, RST_PIN);

// 등록된 카드 UID (4바이트) - 필요시 변경
static const byte authorizedUID[4] = { 0xB6, 0xF7, 0x18, 0xF8 };

// 열림/닫힘 각도(스케치와 동일하게 유지)
static constexpr int OPEN_ANGLE = 30;
static constexpr int CLOSE_ANGLE = 100;

// 초기화: SPI 및 리더 초기화, LED 핀 설정
inline void initNFC() {
  SPI.begin();
  rc522.PCD_Init();
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_NG, OUTPUT);
  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_NG, LOW);
}

// 루프에서 호출하여 카드 감지 및 서보 동작 실행
inline void pollNFC() {
  if (!rc522.PICC_IsNewCardPresent()) {
    // 짧은 지연은 메인 루프가 너무 바쁠 때 과도한 검사 방지
    delay(20);
    return;
  }
  if (!rc522.PICC_ReadCardSerial()) {
    delay(20);
    return;
  }

  // UID 출력
  Serial.print("Card UID: ");
  for (byte i = 0; i < rc522.uid.size; i++) {
    Serial.print(rc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // UID 비교 (최소 4바이트 비교)
  bool ok = (rc522.uid.size >= 4);
  if (ok) {
    for (byte i = 0; i < 4; ++i) {
      if (rc522.uid.uidByte[i] != authorizedUID[i]) { ok = false; break; }
    }
  }

  if (ok) {
    Serial.println("Authorized card - opening door");
    digitalWrite(LED_OK, HIGH);
    digitalWrite(LED_NG, LOW);

    // 열기 -> 대기 -> 닫기 (스케치의 moveServoSmooth 호출)
    moveServoSmooth(OPEN_ANGLE);
    delay(1500);
    moveServoSmooth(CLOSE_ANGLE);

    digitalWrite(LED_OK, LOW);
  } else {
    Serial.println("Unauthorized card");
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_NG, HIGH);
    delay(800);
    digitalWrite(LED_NG, LOW);
  }

  rc522.PICC_HaltA();
  rc522.PCD_StopCrypto1();

  delay(200);
}
// ...existing code...