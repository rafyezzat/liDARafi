#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <TFLI2C.h>
#include <ESP32Servo.h>


// ---------------- TF-Luna ----------------
TFLI2C tflI2C;
int16_t distance_cm;
int16_t tf_addr = TFL_DEF_ADR;

// ---------------- Servo ----------------
Servo servoA;  // GPIO 8
Servo servoB;  // GPIO 9
int angle = 0;
int direction = 1;
unsigned long lastServoTime = 0;
const int servoDelay = 50;  // ms


// ---------------- Wi-Fi ----------------
const char* ssid = "ESP32_LIDAR_AP";
const char* password = "12345678";

WiFiClient client;
WiFiServer server(80);

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 100;

String lastPayload = "NO DATA";



void setupServos() {
  servoA.attach(8, 500, 2400);
  servoB.attach(9, 500, 2400);
}

void connectToAP() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

// Send data to XIAO ESP32-S3
void sendData(int distance) {
  lastPayload = "distance=" + String(distance);

  if (client.connect("192.168.4.1", 80)) {
    client.print("GET /update?distance=");
    client.print(distance);
    client.println(" HTTP/1.1");
    client.println("Host: ESP32_LIDAR_AP");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);

  Wire.begin(6, 7);

  setupServos();
  connectToAP();

  server.begin();
  Serial.println("HTTP server ready");
}

// ---------------- Loop ----------------
void loop() {
  unsigned long now = millis();

  // ----- Servo sweep -----
  if (now - lastServoTime >= servoDelay) {
    lastServoTime = now;

    servoA.write(angle);
    servoB.write(constrain(70 - angle, 0, 70));

    angle += direction;
    if (angle >= 70) direction = -1;
    if (angle <= 0) direction = 1;
  }

  // ----- TF-Luna Read -----
  if (tflI2C.getData(distance_cm, tf_addr)) {

    Serial.print("Dist: "); Serial.println(distance_cm);


    if (now - lastSendTime >= sendInterval) {
      lastSendTime = now;
      sendData(distance_cm);
    }
  }

  // ----- HTTP server: /distance -----
  WiFiClient c = server.available();
  if (c) {
    while (c.connected()) {
      if (c.available()) {
        String req = c.readStringUntil('\r');
        c.readStringUntil('\n');

        if (req.indexOf("GET /distance") >= 0) {
          c.println("HTTP/1.1 200 OK");
          c.println("Content-Type: text/plain");
          c.println("Connection: close");
          c.println();
          c.println(lastPayload);
        }
        break;
      }
    }
    delay(1);
    c.stop();
  }

  delay(1);
}
