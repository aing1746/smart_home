#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>   
#include <SPI.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>

#define RST_PIN 8     // RFID 리더기 RST 핀
#define SS_PIN 53     // RFID 리더기 SS 핀
#define MOTOR_PIN 4   // 서보 모터 핀
#define DHTPIN 2      // DHT 센서 핀
#define DHTTYPE DHT11 // DHT 11



DHT dht(DHTPIN, DHTTYPE);

Servo servo;
MFRC522 rc522(SS_PIN, RST_PIN); // SS, RST 순서

// 블루투스 통신을 위한 객체 생성 (아두이노 19번(RX), 18번(TX) 핀 사용)
SoftwareSerial mySerial(19, 18);  // <RX 핀, TX 핀>
static const int MAG_PIN = 3;     // 리드스위치 핀
static const int BuzzerPin = 7;   // 부저 핀
const int RED_PIN = 9;     // Red LED 핀
const int GREEN_PIN = 10;  // Green LED 핀 (LED 다리: R, GND, G, B)
const int BLUE_PIN = 11;   // Blue LED 핀

// 전역변수로 선언
char c = '0'; // 현재 앱 상태 ('1': 자기장, '2': 온습도, '3': LED ON, '4', '5', '6'...)
String command = ""; // LED 제어 등 문자열 명령어를 위한 변수

// 허용된 UID
static const byte authorizedUID[4] = { 0x83, 0xFE, 0x59, 0x9A };

// 센서 데이터를 읽는 주기를 설정합니다 (밀리초 단위).
const long readingInterval = 2000;
unsigned long lastReadingTime = 0;


int Now_angle = 100;         // 초기 닫힘 각도
const int open_door = 30;    // 문 열림 각도
const int close_door = 100;  // 문 닫힘 각도

const byte ROWS = 4;
const byte COLS = 4;         

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// 키패드 핀 (하드웨어에 맞게 조정)
byte rowPins[ROWS] = {29, 28, 27, 26};      // S1, S2, S3, S4 핀은 각각 29, 28, 27, 26번 핀에 연결 
byte colPins[COLS] = {25, 24, 23, 22};      // R4, R3, R2, R1 핀은 각각 22, 23, 24, 25번 핀에 연결

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// 비밀번호 처리 (Arduino String 사용)
String enteredPassword = "";
const String door_password = "1234";
const unsigned long passwordClearMs = 5000;
unsigned long lastKeyMillis = 0;

// NFC 초기화
void initNFC() {
  SPI.begin();
  rc522.PCD_Init();
}

void initled() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  // Common Anode 기준: 초기 상태 (LED 꺼짐)
  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 255);
}

void initdht() {
  dht.begin();
}

// 서보 초기화
void initServo() {
  servo.attach(MOTOR_PIN);
  servo.write(Now_angle);
}

// 자기 센서 초기화
void initMagnetic() {
  pinMode(MAG_PIN, INPUT);    // 리드스위치 신호핀
  pinMode(BuzzerPin, OUTPUT); // 부저 출력
}

// ============== 수정된 함수 (시작) ==============
// 블루투스 값 읽기
void bt_value() {
  if (mySerial.available()) {
    // 1. 블루투스 데이터를 한 줄씩(\n) 읽어 전역 'command' 변수에 저장
    command = mySerial.readStringUntil('\n'); 
    command.trim(); // 앞뒤 공백 제거

    if (command.length() > 0) {
      if (command.startsWith("C:")) {
        // 2. LED 제어 명령어("C:...")인 경우
        // 'command'에 문자열이 이미 저장됨
        c = '3'; // LED 제어 상태('3')로 즉시 변경
        Serial.println("Got LED Command: " + command);
      } else if (command.length() == 1) {
        // 3. 단일 문자(상태 변경 '1', '2', '4' 등)인 경우
        c = command.charAt(0); // 전역 'c' (상태) 변수 업데이트
        command = ""; // LED 명령어가 아니므로 'command' 비우기
        Serial.println("Got State Command: " + c);
      } else {
        // 기타 알 수 없는 명령은 무시
        Serial.println("Got unknown command: " + command);
        command = ""; 
      }
    }
  }
}
// ============== 수정된 함수 (끝) ==============


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

void pollMagnetic() {
  if (c == '1') { // 자기장 센서 활성화 상태
    if (digitalRead(MAG_PIN) == LOW) {
      // 문 닫힘 (정상)
      noTone(BuzzerPin); // 부저 끄기
      return;
    } else if (digitalRead(MAG_PIN) == HIGH) {
      // 문 열림 (경고)
      tone(BuzzerPin, 100, 2000); // 100Hz 톤, 2초간 울림 (delay 대신)
      // tone(BuzzerPin, 100);    
      // delay(2000); // delay()는 다른 작업을 막으므로 좋지 않습니다.
      // noTone(BuzzerPin);
    }
  } else if (c == '4') { // 자기장 센서 비활성화 상태
    noTone(BuzzerPin); // 상태가 '4'로 바뀌면 부저 끄기
    return;
  }
}


void pollDHT() {
  if(c == '2') { // 온습도 센서 활성화 상태
    // 현재 시간과 마지막 측정 시간 비교하여 2초마다 실행
    if (millis() - lastReadingTime >= readingInterval) {
      lastReadingTime = millis();

      // DHT 센서에서 온습도를 읽습니다.
      float h = dht.readHumidity();
      float t = dht.readTemperature(); // 섭씨 온도

      // 데이터를 시리얼 모니터 (PC)로 출력 (디버깅용)
      Serial.print("Humidity(H): ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature(T): ");
      Serial.print(t);
      Serial.println(" *C");

      // 데이터를 블루투스 모듈로 전송 (앱에서 수신할 형식)
      mySerial.print(t);      // 1. 온도 값 전송
      mySerial.print(",");    // 2. 구분자 콤마 전송
      mySerial.print(h);      // 3. 습도 값 전송
      mySerial.println();     // 4. 줄바꿈 전송
    }
  } else if(c == '5') { // 온습도 센서 비활성화 상태
    return;
  }
}

// ============== 수정된 함수 (시작) ==============
void pollled() {
  if (c == '3') { // '3' = LED 제어 활성화 상태
    // 'C:' 명령어를 수신했을 때만 LED 색상 변경
    if (command.startsWith("C:")) {
      String rgbString = command.substring(2); // "C:" 잘라냄
      int r_index = rgbString.indexOf(':');
      int g_index = rgbString.lastIndexOf(':');

      if (r_index != -1 && g_index != -1 && r_index != g_index) {
        String r_str = rgbString.substring(0, r_index);
        String g_str = rgbString.substring(r_index + 1, g_index);
        String b_str = rgbString.substring(g_index + 1);

        int r = r_str.toInt();
        int g = g_str.toInt();
        int b = b_str.toInt();

        // Common Anode 기준 (255가 끄기, 0이 켜기)
        analogWrite(RED_PIN, 255 - r);
        analogWrite(GREEN_PIN, 255 - g);
        analogWrite(BLUE_PIN, 255 - b);
        
        Serial.print("LED Set: "); Serial.print(r); Serial.print(","); Serial.print(g); Serial.print(","); Serial.println(b);

        command = ""; // ★★★ 중요: 명령어 처리 완료 후 비우기
      }
    }
    // c == '3'이지만 command가 비어있으면(이미 처리했으면)
    // 현재 LED 상태를 '유지'합니다. (끄지 않음)

  } else if (c == '6') { // '6' = LED 끄기 (비활성화)
    // Common Anode LED 기준 (모두 255로 설정하여 끄기)
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 255);
    // c는 '6' 상태로 유지됨
  }
}
// ============== 수정된 함수 (끝) ==============


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);   // Bluetooth 모듈 (핀 18, 19)
  initNFC();      // NFC 초기화 호출
  initServo();    // 서보 초기화 호출
  initMagnetic(); // 자기 센서 초기화 호출
  initdht();      // DHT 센서 초기화 호출
  initled();      // LED 초기화 호출
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

  // --- 메인 폴링 함수들 ---
  bt_value(); // 1. 블루투스 입력 처리 (c, command 변수 업데이트)
  pollMagnetic(); // 2. c 상태에 따라 자기장 센서 처리
  pollDHT();      // 3. c 상태에 따라 온습도 센서 처리
  pollled();      // 4. c 상태 및 command에 따라 LED 처리
  door_open_close(); // 5. RFID 및 키패드에 따른 도어락 처리
}