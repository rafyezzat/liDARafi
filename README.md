# 3D LiDAR Scanner (TF-Luna + VL53L1X + ESP32 + MPU6050)

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/67ae06cc-d147-481a-b0d0-84759e40ba7a" />





This project is a **custom DIY 3D LiDAR system** built using:
- **TF-Luna** (long-range TOF sensor – for main distance)
- **VL53L1X** (short-range TOF sensor – second layer scan)
- **Xiao-seeed ESP32s3** (angle tracking + sensor fusion + sending yaw/pitch)
- **MPU6050** (gyroscope + accelerometer for real yaw/pitch tracking)
- **Stepper motor + TB6600** (automatic rotation, no stepper code)
- **Python + Open3D** for real-time 3D pointcloud visualization

The system scans **360° horizontally** and **~30° vertically**, creating a simple 3D point cloud.

---

## 📸 System Overview

### ✔ Xiao-seeed ESP32s3s:
- Reads **VL53L1X distance**
- Reads **Real Yaw + Real Pitch** from **MPU6050**
- Sends data wirelessly to the PC:
- ssid: "ESP32_LIDAR_AP"
- PASSWORD:"12345678"

### ✔ Arduino UNO does:
- Reads **TF-Luna distance** via I2C
- Sends distance to Python via USB serial

### ✔ Stepper Motor Section:
- Controlled by **555 timer → CD4017** (NOT controlled by code) if u want to use uniploar
- - Controlled by **TB6600** if u want to use bipolar
- - ESP32 only *tracks* angle, does not *drive* the motor

### ✔ PC (Python):
- Reads:
- ESP32 (via HTTP GET) "HTTP://192.168.4.2/"
- TF-Luna (via serial)
- Converts spherical → Cartesian coordinates
- Displays the 3D point cloud using **Open3D**

---

## 📡 Data Format

### From ESP32 WiFi:
Example string received:

- `Yaw`, `Pitch` → **angles for VL53L1X**
- `Dist(mm)` → **VL53L1X distance**
- `Real_Yaw`, `Real_pitch` → **angles used with TF-Luna**

### From Arduino TF-Luna Serial:


---

## 📁 Python Code (Main 3D Visualizer)

> Your code goes here (or add in a `lidar_visualizer.py` file)

---

## 🔌 Hardware Wiring

### 1. TF-Luna → Arduino UNO
- TX → Pin 10
- RX → Pin 11
- 5V → 5V
- GND → GND

### 2. VL53L1X → ESP32
- SDA → GPIO 21
- SCL → GPIO 22
- 3.3V → 3.3V
- GND → GND

### 3. MPU6050 → ESP32
- SDA → 21 (shared)
- SCL → 22 (shared)

### 4. Stepper Motor
- Driven by **ULN2003** board
- Pulsed by **555 timer + CD4017**

### 5. IR Sensor
- Output → ESP32 GPIO for zero-point reset

---

## 🗺 Features

✔ Real-time 3D mapping  
✔ Dual-sensor fusion (TF-Luna + VL53L1X)  
✔ Wireless orientation tracking  
✔ No slip rings  
✔ No stepper-motor code required  
✔ Python + Open3D visualization  
✔ Works at ~10–20Hz scan rate  

---

## 📷 Demo / Screenshots

---

## 📦 Future Improvements
- Kalman Filter for smoother angles  
- Colored point cloud  
- SLAM integration  
- ESP32-C3 or Raspberry Pi for faster processing  

---

## 🧑‍💻 Author
**Rafy** – Robotics & Embedded Systems Developer  


