#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int ROW_NUM = 4; // Number of rows
const int COL_NUM = 4; // Number of columns

char keys[ROW_NUM][COL_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROW_NUM] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COL_NUM] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROW_NUM, COL_NUM);
Servo lockServo;

const int servoervoPin = 4;
const int buzzerPin = 2;

const char correctPassword[] = "1234"; // Set your password here

String enteredPassword = "";

bool lock = true; // Start locked
bool wrongPasswordEntered = false;
bool buzzerOn = false;

void setup() {
  lockServo.attach(servoervoPin);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  lockServo.write(90); // Lock position
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    buzz();
    if (key == 'A') {
      enteredPassword = "";
      lock();
    } else {
      enteredPassword += key;
      if (enteredPassword.length() == strlen(correctPassword)) {
        if (enteredPassword == correctPassword) {
          unlock();
        } else {
      wrongPassword();
      enteredPassword = "";
      lcd.setCursor(0, 1);
      lcd.print("Enter Password:             ");
        }
      }
    }

lcd.setCursor(0, 1);
lcd.print("Enter: ");
for (int i = 0; i < enteredPassword.length(); i++) {
  if (i > enteredPassword.length() - 1) {
    lcd.print("*");
  } else {
  lcd.print(enteredPassword.charAt[i]);
  }
}

if (enteredPassword.length() == 4) {
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Entered:       ")
  }
}

  if (buzzerOn) {
    buzz();
    buzzerOn = false; 
  }
} 

void lock() {
  lockServo.write(90); // Lock position
  lock = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Locked");
  lcd.setCursor(0, 1);
  lcd.print("Entered:     ");
  Serial.println("Door Locked");
}

void unlock() {
  lockServo.write(0); // Unlock position
  lock = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Unlocked");
  lcd.setCursor(0, 1);
  lcd.print("Entered:     " + enteredPassword);
  Serial.println("Door Unlocked");

  tone(buzzerPin, 200, 50);
  delay(200);
  noTone(buzzerPin);

  enteredPassword = "";
}

void buzz() {
  tone(buzzerPin, 200, 50); // Start buzzer
  delay(50);            // Buzz for 100 ms
  noTone(buzzerPin);     // Stop buzzer
}

void wrongPassword() {
  lockServo.write(90); // Lock position
  lock = true;
  buzzerOn = true; 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong Password");
  lcd.setCursor(0, 1);
  lcd.print("Entered:     ");
  Serial.println("Wrong Password Entered");

  tone(buzzerPin, 200, 50);
  delay(500);
  noTone(buzzerPin);
}


