#include <Servo.h>   
#include <MFRC522.h>
#include <SPI.h>
#include <Keypad.h>

#define RST_PIN 8
#define SS_PIN 9
#define MOTOR_PIN 4

Servo servo;
MFRC522 rc522(SS_PIN, RST_PIN); // SS, RST 순서

// 허용된 UID
static const byte authorizedUID[4] = { 0x83, 0xFE, 0x59, 0x9A };

int Now_angle = 100;   // 초기 닫힘 각도
const int open_door = 30;
const int close_door = 100;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// 키패드 핀 (하드웨어에 맞게 조정)
byte rowPins[ROWS] = {29, 28, 27, 26};
byte colPins[COLS] = {25, 24, 23, 22};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// 비밀번호 처리 (Arduino String 사용)
String enteredPassword = "";
const String door_password = "1234";
const unsigned long passwordClearMs = 5000;
unsigned long lastKeyMillis = 0;

void initNfc() {
  SPI.begin();
  rc522.PCD_Init();
}

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
      // 카드로 연 경우 비밀번호는 초기화해 둠
      enteredPassword = "";
    } else {
      Serial.println("Unregistered card - KEEP/CLOSE");
      moveServoTo(close_door);
    }

    rc522.PICC_HaltA();
    rc522.PCD_StopCrypto1();
    delay(200); // 카드 처리 후 짧은 지연
    return;
  }

  // 카드가 없더라도 비밀번호가 맞으면 열기
  if (enteredPassword == door_password) {
    Serial.println("Password correct - OPEN");
    moveServoTo(open_door);
    delay(1500);
    enteredPassword = "";
    return;
  }

  // 카드/비밀번호 모두 없으면 문 닫기
  if (Now_angle != close_door) {
    Serial.println("No card/password - CLOSE");
    moveServoTo(close_door);
  }
  // 루프 과도한 점유 방지
  delay(50);
}

void setup() {
  Serial.begin(9600);
  initNfc();
  initServo();
}

void loop() {
  // 키패드 처리: 숫자 누르면 enteredPassword에 추가, '#' 누르면 초기화, '*'는 취소
  char k = keypad.getKey();
  if (k) {
    Serial.print("Keypad: ");
    Serial.println(k);
    lastKeyMillis = millis();
    if (k >= '0' && k <= '9') {
      if (enteredPassword.length() < 8) enteredPassword += k;
    } else if (k == '#') {
      // '#' 입력 시 비밀번호 초기화
      enteredPassword = "";
      Serial.println("Password cleared (#)");
    } else if (k == '*') {
      // '*' 입력 시 취소(초기화)
      enteredPassword = "";
      Serial.println("Password cleared (*)");
    }
  }

  // 마지막 키 입력 후 일정 시간이 지나면 입력 초기화
  if (enteredPassword.length() > 0 && millis() - lastKeyMillis > passwordClearMs) {
    enteredPassword = "";
    Serial.println("Password auto-cleared");
  }

  door_open_close();
}