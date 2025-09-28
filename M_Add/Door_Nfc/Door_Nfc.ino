#include <Servo.h>   
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN 8
#define SS_PIN 9
#define MOTOR_PIN 4

Servo servo;
MFRC522 rc522(SS_PIN, RST_PIN); // SS, RST 순서

// 허용된 UID (원래 코드 값 유지)
static const byte authorizedUID[4] = { 0x83, 0xFE, 0x59, 0x9A };

int Now_angle = 100;   // 초기 닫힘 각도
const int open_door = 30;
const int close_door = 100;

void initNfc() {
  SPI.begin();
  rc522.PCD_Init();
}

void initServo() {
  servo.attach(MOTOR_PIN);
  servo.write(Now_angle);
}

void door_open_close() {
  if (!rc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!rc522.PICC_ReadCardSerial()) {
    return;
  }

  // UID 비교 (4바이트)
  bool authorized = (rc522.uid.size >= 4);
  if (authorized) {
    for (byte i = 0; i < 4; ++i) {
      if (rc522.uid.uidByte[i] != authorizedUID[i]) { authorized = false; break; }
    }
  }

  if (authorized) {
    Serial.println("<< OK !!! >>  Registered card...");
    for (int a = close_door; a >= open_door; --a) {
      servo.write(a);
      delay(10);
    }
  } else {
    Serial.println("<< WARNING !!! >>  This card is not registered");
    delay(500);
  }

  rc522.PICC_HaltA();
  rc522.PCD_StopCrypto1();
}

void setup() {
  Serial.begin(9600);
  initNfc();
  initServo();
}

void loop() {
  door_open_close();
}