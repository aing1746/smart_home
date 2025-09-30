#include <SoftwareSerial.h>
#include "DHT.h"

#define BT_RXD 5
#define BT_TXD 4
SoftwareSerial bluetooth(BT_RXD, BT_TXD);

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define LED_G 8
#define LED_R 9
#define LED_Y 10
int led_mod = 1;

void setup() {
  // put your setup code here, to run once:
  bluetooth.begin(9600);
  dht.begin();
  Serial.begin(9600);
  delay(1000);

  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
}

void LED_ON(int G, int R, int Y)
{
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_Y, LOW);
  if (G > 0) {
    digitalWrite(LED_G, HIGH);
  }
  if (R > 0) {
    digitalWrite(LED_R, HIGH);
  }
  if (Y > 0) {
    digitalWrite(LED_Y, HIGH);
  }
}


void loop() {
  delay(1000);
  //LED_ON(led_mod & 0x01, led_mod & 0x02, led_mod & 0x04);
  //led_mod++;
  //LED_ON(led_mod & 0x01, led_mod & 0x02, led_mod & 0x04);
  //led_mod = led_mod << 1;
  //if (led_mod > 0x04)
  //  led_mod = 1;

  // put your main code here, to run repeatedly:
  char rec_data = 0;
  if (bluetooth.available()) {
    rec_data = bluetooth.read();
    Serial.println(rec_data);
  }
  
  if (rec_data == '1') {
    LED_ON(1, 0, 0);
  }
  else if (rec_data == '2') {
    LED_ON(0, 1, 0);
  }
  else if (rec_data == '3') {
    LED_ON(0, 0, 1);
  }

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  char charbuf[20] = {0};
  if (rec_data == 't') {
    String c = String(t);
    c.toCharArray(charbuf, c.length());
    bluetooth.write(charbuf);
    Serial.println(charbuf);
  }
  else if (rec_data == 'h') {
    String c = String(h);
    c.toCharArray(charbuf, c.length());
    bluetooth.write(charbuf);
    Serial.println(charbuf);
  }
}
