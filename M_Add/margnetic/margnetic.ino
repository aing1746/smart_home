#include "margnetic.h"

void setup() {
  Serial.begin(9600);
  initMagnetic(); // 헤더에 정의한 초기화 함수 호출
}

void loop() {
  pollMagnetic(); // 헤더에 정의한 폴링 함수 호출
}