static const int MAG_PIN = 3;
static const int LED_PIN = 11; // PWM 가능한 핀으로 변경 (UNO: 3,5,6,9,10,11)
  
void initMagnetic() {
    pinMode(MAG_PIN, INPUT); // 리드스위치 신호핀
    pinMode(LED_PIN, OUTPUT); // LED 출력
}

void pollMagnetic() {
    if (digitalRead(MAG_PIN) == HIGH) {
      Serial.println("mag, HIGH");
      return;
    } else if (digitalRead(MAG_PIN) == LOW) {
      // tone(BuzzerPin, 100);    // 100Hz 톤
      LED_ON();
      delay(2000);               // 2초
      // noTone(BuzzerPin);
      LED_OFF();
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

void setup() {
  Serial.begin(9600);
  initMagnetic(); // 초기화 호출

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // pinMode(LED_PIN, OUTPUT); // initMagnetic에서 설정됨
}

void loop() {
  pollMagnetic(); // 폴링 호출
}