#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_VL53L1X.h>
#include <WiFi.h>

MPU6050 mpu(Wire);
Adafruit_VL53L1X lox = Adafruit_VL53L1X();

float yaw = 0;
float gyroZ_offset = 0;
unsigned long lastTime = 0;

uint16_t distance = 0;  // last valid distance
float real_lidar_pitch;
float real_lidar_yaw;

// Wi-Fi AP
const char* ssid = "ESP32_LIDAR_AP";
const char* password = "12345678"; // min 8 chars
WiFiServer server(80);

// ------------------- MPU6050 CALIBRATION -------------------
void calibrateGyro() {
  Serial.println("Calibrating Gyro... keep MPU6050 still!");
  float sum = 0;
  int samples = 2000;  // more samples for better calibration

  for (int i = 0; i < samples; i++) {
    mpu.update();
    sum += mpu.getGyroZ();
    delay(2);
  }

  gyroZ_offset = sum / samples;
  Serial.print("Gyro Z offset = ");
  Serial.println(gyroZ_offset, 5);
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // MPU6050 Init
  if (mpu.begin() != 0) {
    Serial.println("MPU6050 not found!");
    while (1);
  }
  delay(1000);
  calibrateGyro();
  lastTime = micros();

  // VL53L1X Init
  if (!lox.begin()) {
    Serial.println("VL53L1X not found!");
    while (1);
  }
  lox.startRanging();

  // Wi-Fi AP Setup
  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
  Serial.println("Server started! Connect a device to this AP.");
}

// ------------------- LOOP -------------------
void loop() {
  // ---- Update MPU6050 ----
  mpu.update();

  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  // ---- Yaw calculation ----
  float gyroZ = mpu.getGyroZ() - gyroZ_offset;
  yaw += gyroZ * dt;
  real_lidar_yaw = 180 + yaw;
  if (yaw < 0) yaw += 360;
  if (yaw >= 360) yaw -= 360;
  if (real_lidar_yaw < 0) real_lidar_yaw += 360;
  if (real_lidar_yaw >= 360) real_lidar_yaw -= 360;

  float pitch = -mpu.getAngleX();
  real_lidar_pitch = (2 * pitch) - 90;

  // ---- VL53L1X distance ----
  if (lox.dataReady()) {
    uint32_t dist_mm = 0;
    lox.GetDistance(&dist_mm);
    distance = (uint16_t)dist_mm;
    lox.clearInterrupt();
  }

  // ---- Prepare data string ----
  String data = "Yaw: " + String(yaw,2) +
                " | Pitch: " + String(pitch,2) +
                " | Dist(mm): " + String(distance) +
                " | GyroZ: " + String(mpu.getGyroZ(),2) +
                " | AccX: " + String(mpu.getAccX(),2) +
                " | Real_Yaw: " + String(real_lidar_yaw,2) +
                " | Real_pitch: " + String(real_lidar_pitch,2);

  Serial.println(data);

  // ---- Handle Wi-Fi clients ----
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // Send HTTP response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/plain");
          client.println("Connection: close");
          client.println();
          client.println(data);  // send sensor data
          break;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client Disconnected");
  }

  delay(10); // small loop delay
}
