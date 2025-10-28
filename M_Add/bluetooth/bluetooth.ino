#include <SoftwareSerial.h>

// 블루투스 통신을 위한 객체 생성 (아두이노 19번(RX), 18번(TX) 핀 사용)
SoftwareSerial mySerial(19, 18);  // <RX 핀, TX 핀>
static const int MAG_PIN = 3;
static const int BuzzerPin = 7;

// 전역변수로 선언
char c = '0';

void setup() {
  Serial.begin(9600);     // PC 시리얼 모니터
  Serial1.begin(9600);   // Bluetooth 모듈 (핀 18, 19)
  initMagnetic(); // 초기화 호출
}

void initMagnetic() {
  pinMode(MAG_PIN, INPUT); // 리드스위치 신호핀
  pinMode(BuzzerPin, OUTPUT); // 부저 출력
}


void bt_vlalue() {
  if (Serial1.available()) {  // Serial1 -> mySerial로 변경
    c = Serial1.read();  // 전역변수 c에 저장
  }
}

void pollMagnetic() {
  if (c == '1') {
    if (digitalRead(MAG_PIN) == LOW) {
      Serial.println(c);
      return;
    } else if (digitalRead(MAG_PIN) == HIGH) {
      tone(BuzzerPin, 100);    // 100Hz 톤
      delay(2000);               // 2초
      noTone(BuzzerPin);
    }
  } else if (c == '4') {
    return;
  }
}

void loop() {
  bt_vlalue();
  pollMagnetic();
}
