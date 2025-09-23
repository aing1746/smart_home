#pragma once

static const int BuzzerPin = 8;
static const int MAG_PIN = 3;
static const int numTones = 8;
static const int tones[numTones] = {1479, 523, 1479, 523, 1479, 523, 1479, 523};

// setup() 대신 메인 스케치의 setup()에서 한 번 호출하세요
inline void initMagnetic() {
    pinMode(MAG_PIN, INPUT); // 리드스위치 신호핀
    pinMode(BuzzerPin, OUTPUT);
}

// loop() 대신 메인 스케치의 loop()에서 주기적으로 호출하세요
inline void pollMagnetic() {
    // 리드스위치가 HIGH 면 Buzzer OFF
    if (digitalRead(MAG_PIN) == HIGH) {
        // 아무 동작 없음
        return;
    }
    // 리드스위치가 LOW면 Buzzer ON
    for (int i = 0; i < numTones; i++) {
        tone(BuzzerPin, tones[i]);
        delay(500);
    }
    noTone(BuzzerPin);
    delay(500);
}