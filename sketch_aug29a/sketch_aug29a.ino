#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int ROW_NUM = 4; // 키패드의 행 수
const int COLUMN_NUM = 4; // 키패드의 열 수

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
}; // 키패드의 각 버튼에 대한 값

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; // 행(Row) 핀에 연결된 아두이노 핀 번호
byte pin_column[COLUMN_NUM] = {10, 11, 12, 13}; // 열(Column) 핀에 연결된 아두이노 핀 번호

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
Servo lockServo;
const int servoPin = 4;
const int buzzerPin = 2;

const char* correctPassword = "0854"; // 설정된 비밀번호

String enteredPassword = "";
bool locked = true;
bool wrongPasswordEntered = false;
bool buzzerOn = false;

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 주소와 행, 열의 수 설정

// 함수 프로토타입 추가 (loop()에서 사용되는 함수들)
void buzz();
void lock();
void unlock();
void wrongPassword();

void setup() {
  lockServo.attach(servoPin);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  Serial.begin(9600);

  lcd.init(); // LCD 초기화
  lcd.backlight(); // LCD 백라이트 활성화
  lcd.setCursor(0, 0);
  lcd.print("Door Locked"); // 초기 상태 출력

  lockServo.write(90); // 서보모터를 잠금 상태로 셋팅
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    buzz(); // 키패드 입력 시 피에조 부저에서 소리 재생
    if (key == 'A') { // A 버튼을 누르면 잠금장치 잠금
      enteredPassword = "";
      lock();
    } else {
      enteredPassword += key;
      if (enteredPassword.length() == strlen(correctPassword)) {
        if (enteredPassword == correctPassword) { // 비밀번호 일치 여부 확인
          unlock();
        } else {
          wrongPassword();
          enteredPassword = "";
          lcd.setCursor(0, 1);
          lcd.print("Entered:        "); // 숫자 입력이 잘못되었을 때 초기화
        }
      }
    }

    // LCD에 현재 입력된 번호 표시
    lcd.setCursor(0, 1);
    lcd.print("Entered: ");
    for (int i = 0; i < enteredPassword.length(); i++) {
      if (i < enteredPassword.length() - 1) {
        lcd.print("*");
      } else {
        lcd.print(enteredPassword[i]);
      }
    }
  }
}

// buzzer 소리 재생 함수 추가
void buzz() {
  // 짧은 비프음 재생
  tone(buzzerPin, 200, 50);
  delay(50);
  noTone(buzzerPin);
}

void lock() {
  lockServo.write(90);
  locked = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Locked");
  lcd.setCursor(0, 1);
  lcd.print("Entered:        ");
  enteredPassword = "";
  noTone(buzzerPin);
}

void unlock() {
  lockServo.write(0);
  locked = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Unlocked");
  lcd.setCursor(0, 1);
  lcd.print("Entered:        ");
  tone(buzzerPin, 1000, 150);
  delay(150);
  noTone(buzzerPin);
  enteredPassword = "";
}

void wrongPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong Password");
  lcd.setCursor(0, 1);
  lcd.print("Try again");
  tone(buzzerPin, 1000, 300);
  delay(300);
  noTone(buzzerPin);
}
