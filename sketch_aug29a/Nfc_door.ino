// ...existing code...
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// 핀 설정 (필요시 변경)
constexpr int SERVO_PIN = 6;   // 서보 (이 파일에서 사용)
constexpr int RST_PIN = 5;     // MFRC522 RST
constexpr int SS_PIN  = 10;    // MFRC522 SS
constexpr int LED_OK  = 4;     // 인식 LED
constexpr int LED_NG  = 3;     // 미인식 LED

// 서보/리더 선언
Servo servo;
MFRC522 rc522(SS_PIN, RST_PIN);

float current_angle = 0;
constexpr float MIN_ANGLE = 0;
constexpr float MAX_ANGLE = 180;

// 문 동작 각도(원하시면 조정)
constexpr int OPEN_ANGLE = 30;
constexpr int CLOSE_ANGLE = 100;

// 등록된 카드 UID (4바이트) - 필요시 변경
const byte authorizedUID[4] = { 0xB6, 0xF7, 0x18, 0xF8 };

// 부드럽게 이동하는 유틸
void moveServoSmooth(float target, int steps = 50, int stepDelayMs = 10) {
  target = constrain(target, MIN_ANGLE, MAX_ANGLE);
  float diff = target - current_angle;
  for (int i = 0; i < steps; ++i) {
    current_angle += diff / steps;
    servo.write(static_cast<int>(current_angle));
    delay(stepDelayMs);
  }
  current_angle = target;
  servo.write(static_cast<int>(current_angle));
}

void setup() {
  Serial.begin(9600);
  delay(50);
  SPI.begin();
  rc522.PCD_Init();

  servo.attach(SERVO_PIN);
  servo.write(static_cast<int>(current_angle));

  pinMode(LED_OK, OUTPUT);
  pinMode(LED_NG, OUTPUT);

  digitalWrite(LED_OK, LOW);
  digitalWrite(LED_NG, LOW);

  Serial.println("Smart Home NFC -> Servo ready");
  Serial.println("Commands: A<angle> to set angle manually (e.g. A90)");
}

void loop() {
  // 시리얼 수동 제어 처리
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 0 && cmd.charAt(0) == 'A') {
      int val = cmd.substring(1).toInt();
      val = constrain(val, static_cast<int>(MIN_ANGLE), static_cast<int>(MAX_ANGLE));
      Serial.print("Manual move to "); Serial.println(val);
      moveServoSmooth(val);
    }
  }

  // NFC 카드 감지
  if (!rc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  }
  if (!rc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  // UID 출력
  Serial.print("Card UID: ");
  for (byte i = 0; i < rc522.uid.size; i++) {
    Serial.print(rc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // UID 비교 (4바이트만 비교)
  bool ok = true;
  if (rc522.uid.size < 4) ok = false;
  else {
    for (byte i = 0; i < 4; ++i) {
      if (rc522.uid.uidByte[i] != authorizedUID[i]) { ok = false; break; }
    }
  }

  if (ok) {
    Serial.println("Authorized card - opening door");
    digitalWrite(LED_OK, HIGH);
    digitalWrite(LED_NG, LOW);

    // 열기 -> 대기 -> 닫기
    moveServoSmooth(OPEN_ANGLE);
    delay(1500); // 열린 상태 유지 시간 (필요시 조정)
    moveServoSmooth(CLOSE_ANGLE);

  } else {
    Serial.println("Unauthorized card");
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_NG, HIGH);
    delay(800);
    digitalWrite(LED_NG, LOW);
  }

  // 카드 정지 및 짧은 대기
  rc522.PICC_HaltA();
  rc522.PCD_StopCrypto1();
  delay(300);
}