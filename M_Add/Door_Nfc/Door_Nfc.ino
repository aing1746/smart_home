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

// NFC 초기화
void initNfc() {
  SPI.begin();
  rc522.PCD_Init();
}

// 서보 초기화
void initServo() {
  servo.attach(MOTOR_PIN);
  servo.write(Now_angle);
}

// 서보를 부드럽게 목표 각도로 이동하고 Now_angle 갱신
void moveServoTo(int target) {
  target = constrain(target, 0, 180);
  if (target == Now_angle) return;
  if (target > Now_angle) {
    for (int a = Now_angle + 1; a <= target; ++a) {
      servo.write(a);
      delay(8);
    }
  } else {
    for (int a = Now_angle - 1; a >= target; --a) {
      servo.write(a);
      delay(8);
    }
  }
  Now_angle = target;
}

void door_open_close() {
  // 카드가 새로 있는지 검사 및 읽기 시도
  if (rc522.PICC_IsNewCardPresent() && rc522.PICC_ReadCardSerial()) {

    // UID 비교 (4바이트)
    bool authorized = (rc522.uid.size >= 4);
    if (authorized) {
      for (byte i = 0; i < 4; ++i) {
        if (rc522.uid.uidByte[i] != authorizedUID[i]) { authorized = false; break; }
      }
    }

    if (authorized) {
      Serial.println("Registered card - OPEN");
      moveServoTo(open_door);
      delay(1500); // 열린 시간
    } else {
      Serial.println("Unregistered card - KEEP/CLOSE");
      moveServoTo(close_door);
    }

    rc522.PICC_HaltA();
    rc522.PCD_StopCrypto1();
    delay(200); // 카드 처리 후 짧은 지연
    return;
  }

  // 카드가 감지되지 않으면 문 닫기 (닫혀 있지 않다면 닫음)
  if (Now_angle != close_door) {
    Serial.println("Registered card - CLOSE");
    moveServoTo(close_door);
  }
  delay(50); // 루프 과도한 점유 방지
}

void setup() {
  Serial.begin(9600);
  initNfc();
  initServo();
}

void loop() {
  door_open_close();
}