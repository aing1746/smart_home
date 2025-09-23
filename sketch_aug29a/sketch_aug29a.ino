#include <Servo.h>   
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN 8
#define SS_PIN 9

Servo servo;
MFRC522 rc522(RST_PIN, SS_PIN);

class Nfc {
  public:
    void setup() {
      SPI.begin();
      rc522.PCD_Init();
      // authorized UID 예시 (필요하면 사용)
      static const byte authorizedUID_NFC[4] = { 0x83, 0xFE, 0x59, 0x9A };
    }
};
Nfc nfc;

class Ser_motor : public Nfc {
  protected:
    const int motorPin = 4;
    int Now_angle = 100;   // 초기 닫힘 각도
    int open_door = 30;
    int close_door = 100;

  public:
    void setup() {
      servo.attach(motorPin);
      servo.write(Now_angle); 
    } 

    void door_open_close() {
      if ( !rc522.PICC_IsNewCardPresent() || !rc522.PICC_ReadCardSerial() ) {
        return;
      }
      if (rc522.uid.uidByte[0]==0x83 && rc522.uid.uidByte[1]==0xFE && rc522.uid.uidByte[2]==0x59 
          && rc522.uid.uidByte[3]==0x9A) {
        Serial.println("<< OK !!! >>  Registered card...");
        for (int a = close_door; a >= open_door; --a) {
          servo.write(a);
          delay(10);
        }
      } else {
        Serial.println("<< WARNING !!! >>  This card is not registered");
        delay(500);
      }
    }

    void loop() {
      door_open_close();  
    }
};
Ser_motor ser_motor;

void setup() {
  Serial.begin(9600);
  nfc.setup();
  ser_motor.setup();
}

void loop() {
  ser_motor.loop();
}