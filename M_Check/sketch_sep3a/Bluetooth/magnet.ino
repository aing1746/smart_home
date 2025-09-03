int LED = 2; // LED 핀

void setup(){
  Serial.begin(9600);
  pinMode(3, INPUT); // 리드스위치 신호핀
  pinMode(LED, OUTPUT);
}

void loop(){
// 리드스위치가 HIGH 면 LED OFF
  if(digitalRead(3) == 1){
    digitalWrite(LED, LOW);
  }
// 리드스위치가 LOW면 LED ON
  else{
    digitalWrite(LED, HIGH);
  }
  delay(500);
}

