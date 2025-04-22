#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust to 0x3F if needed

const int trigPin = 9;
const int echoPin = 10;
const int irPin = 4;
const int servoPin = 3;

long duration;
int distance;

bool gateOpen = false;
unsigned long lastCloseTime = 0;
const unsigned long cooldown = 5000; // 5 sec delay

void setup() {
  myservo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irPin, INPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  myservo.write(0);  // Start with gate closed
}

void loop() {
  if (millis() - lastCloseTime < cooldown) {
    return;  // Wait during cooldown
  }

  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  // If a car is detected and gate is not open
  if (distance < 30 && !gateOpen) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car Detected!");
    lcd.setCursor(0, 1);
    lcd.print("Opening Gate...");

    myservo.write(120); // Open gate
    gateOpen = true;
    delay(2000);

    // Wait for car to pass (IR sensor HIGH means clear)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car Under Gate");

    while (digitalRead(irPin) == LOW) {
      delay(100);
    }

    // Car has passed
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Closing Gate...");
    myservo.write(0); // Close gate
    gateOpen = false;
    lastCloseTime = millis();  // Start cooldown
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please Wait...");
  }

  delay(50); // Prevent fast looping
}
