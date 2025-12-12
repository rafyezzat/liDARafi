#include <Servo.h>
#include <Wire.h>
#include <TFLI2C.h>

TFLI2C tfl;

Servo servoA;
Servo servoB;

#define STEP 4
#define DIR  3

int angle = 0;
int direction = 1;
unsigned long lastServoTime = 0;
const int servoDelay = 50;

unsigned long lastStepTime = 0;
const int stepDelay = 500;

int16_t dist;
int16_t strength;
int16_t temp;

void setup() {
  Serial.begin(115200);

  servoA.attach(8, 500, 2400);
  servoB.attach(9, 500, 2400);

  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  digitalWrite(DIR, HIGH);

  Wire.begin(); // SDA=A4, SCL=A5

  Serial.println("System Ready");
}

void loop() {
  unsigned long now = millis();

  // Servo move
  if (now - lastServoTime >= servoDelay) {
    lastServoTime = now;

    servoA.write(angle);
    int angle2 = 70 - angle;
    angle2 = constrain(angle2, 0, 70);
    servoB.write(angle2);

    angle += direction;
    if (angle >= 70) direction = -1;
    if (angle <= 0)  direction = 1;
  }

  // Stepper
  unsigned long nowMicros = micros();
  if (nowMicros - lastStepTime >= stepDelay) {
    lastStepTime = nowMicros;
 // Proper STEP pulse
    digitalWrite(STEP, HIGH);
    delayMicroseconds(5);      // pulse width HIGH
    digitalWrite(STEP, LOW);
    delayMicroseconds(5);      // pulse width LOW
      }

  // TF-Luna I2C read (4 arguments)
  if (tfl.getData(dist, strength, temp, TFL_DEF_ADR)) {
    Serial.print("Distance: ");
    Serial.print(dist);
    Serial.print(" cm   Strength: ");
    Serial.print(strength);
    Serial.print("   Temp: ");
    Serial.print(temp / 100.0);
    Serial.println(" °C");
  } else {
    Serial.println("TF-Luna: No data");
  }
}
