#include <SoftwareSerial.h>

// 블루투스 통신을 위한 객체 생성 (아두이노 19번(RX), 18번(TX) 핀 사용)
// <RX 핀, TX 핀>
SoftwareSerial mySerial(19, 18);
static const int MAG_PIN = 3;
static const int LED_PIN = 11; // PWM 가능한 핀으로 변경
static const int BuzzerPin = 7;

// 전역변수로 선언
char c = '0';

void setup() {
  Serial.begin(9600);     // PC 시리얼 모니터
  Serial1.begin(9600);    // 블루투스 모듈 (핀 18, 19)
  initMagnetic(); // 초기화 호출
  
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void initMagnetic() {
  pinMode(MAG_PIN, INPUT); // 리드스위치 신호핀
  pinMode(LED_PIN, OUTPUT); // LED 출력
  pinMode(BuzzerPin, OUTPUT); // 부저 출력
}

void pollMagnetic() {
  if (c == '1') {
    if (digitalRead(MAG_PIN) == LOW) {
      Serial.println("mag, HIGH");
      return;
    } else if (digitalRead(MAG_PIN) == HIGH) {
      tone(BuzzerPin, 100);    // 100Hz 톤
      LED_ON();
      delay(2000);               // 2초
      noTone(BuzzerPin);
      LED_OFF();
    }
  } else if (c == '4') {
    return;
  }
}

void LED_ON() {
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
}

void LED_OFF() {
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void bt_vlalue() {
  if (Serial1.available()) {
    c = Serial1.read();  // 전역변수 c에 저장
  }
}

void loop() {
  bt_vlalue();
  pollMagnetic();
  
  if (c == '2') {
    LED_ON();
  } else if (c == '5') {
    LED_OFF();
  }
  
  if (c == '3') {
    LED_ON();
  } else if (c == '6') {
    LED_OFF();
  }
}