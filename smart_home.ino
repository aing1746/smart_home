#include <Servo.h>
#include "door.h"


// 서보 핀 설정
constexpr int SERVO_PIN = 9;   // 필요 시 변경

Servo servo;

// 스윕 관련 변수
float current_angle = 0;      // 현재 각도
float step_dir = 1;           // 이동 방향 (1: 증가, -1: 감소)
constexpr float MIN_ANGLE = 0;   // 최소 각도
constexpr float MAX_ANGLE = 180; // 최대 각도 (서보 스펙에 맞게 조정)
constexpr unsigned long STEP_INTERVAL = 15; // ms, 각도 변경 주기
unsigned long last_step_time = 0; // 마지막 각도 변경 시각

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  servo.write(current_angle);
  Serial.println("Servo test start");
  Serial.println("Commands:");
  Serial.println("  A<angle>   -> set angle (0-180)");
}

void loop() {
  handle_serial();

}

void handle_serial() {
  if (!Serial.available()) return; // 수신 데이터 없으면 종료

  String cmd = Serial.readStringUntil('\n'); // 개행까지 명령 읽기
  cmd.trim(); // 앞뒤 공백 제거
  if (cmd.length() == 0) return; // 빈 문자열 무시

  if (cmd.charAt(0) == 'A') { // A명령 처리 (각도 설정)
    int val = cmd.substring(1).toInt(); // 숫자 부분 파싱
    val = constrain(val, MIN_ANGLE, MAX_ANGLE); // 범위 제한
    // current_angle = val;

    constexpr float div = 50; // 부드러운 이동을 위한 분할 단계
    const float diff_angle = val - current_angle;

    for (int i = 0; i < div; ++i) {
        servo.write(static_cast<int>(current_angle += diff_angle / div));
        Serial.print("Move -> "); Serial.println(current_angle);
    }

    return;
  }

  Serial.println("Unknown command"); // 알 수 없는 명령
}
