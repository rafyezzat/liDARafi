#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <VL53L1X.h>
#include <MPU6050_light.h>

// === Custom I2C buses ===
TwoWire I2C_VL53 = TwoWire(0);  // Default bus for VL53L1X
TwoWire I2C_MPU  = TwoWire(1);  // Secondary I2C for MPU6050

// === Sensor objects ===
VL53L1X distanceSensor;
MPU6050 mpu(I2C_MPU);
// ====== Wi-Fi Credentials of ESP32-C3 AP ======
const char* ssid = "Car_Controller";
const char* password = "12345678";

// ====== ESP32-C3 SuperMini IP (Server) ======
const char* host = "192.168.4.1";

// === Angle Tracking ===
float currentAngle = 0;
unsigned long lastTime = 0;
bool mpuReady = false;
/*
// === HTTP handler ===
void handleData() {
  int distance = distanceSensor.read();
  String payload = "Angle: " + String(currentAngle, 2) + "°, Distance: " + String(distance) + " mm";
  server.send(200, "text/plain", payload);
}*/

void setup() {
  Serial.begin(115200);
  delay(500);

  // === Init both I2C buses ===
  I2C_VL53.begin(21, 22);  // VL53L1X I2C
  I2C_MPU.begin(17, 16);   // MPU6050 I2C

  // === VL53L1X Init ===
  distanceSensor.setBus(&I2C_VL53);
  distanceSensor.setTimeout(500);
  if (!distanceSensor.init()) {
    Serial.println("VL53L1X not found!");
    while (1);
  }
  distanceSensor.setDistanceMode(VL53L1X::Long);
  distanceSensor.startContinuous(50);  // 20Hz

  // === MPU6050 Init ===
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 error: ");
    Serial.println(status);
    while (1);
  }
  Serial.println("MPU6050 connected");
  mpu.calcOffsets();  // calibrate gyro/accel
  mpuReady = true;
  lastTime = millis();
/*
  // === Start Wi-Fi Access Point ===
  WiFi.softAP(ssid, password, 6);  // Channel 6
  delay(2000);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());*/
    // ====== Wi-Fi Connect to ESP32-C3 SuperMini ======
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

}

void loop() {

  if (!mpuReady) return;

  mpu.update();
  int distance = distanceSensor.read();
  // === Integrate gyroZ into current angle ===
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  float gyroZ = mpu.getGyroZ();
  currentAngle += gyroZ * dt;

  // Normalize angle
  if (currentAngle >= 360.0) currentAngle -= 360.0;
  else if (currentAngle < 0.0) currentAngle += 360.0;

    // Send via HTTP GET
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(host) + "/receive?angle=" + String(currentAngle, 2) + "&distance=" + String(distance);
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Sent → " + url);
      String response = http.getString();
      Serial.println("Response ← " + response);
    } else {
      Serial.println("HTTP failed");
    }
    http.end();
  }

  delay(5);
}
