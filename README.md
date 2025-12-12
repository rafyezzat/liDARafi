# 3D LiDAR Scanner (TF-Luna + VL53L1X + ESP32 + MPU6050)

This project is a **custom DIY 3D LiDAR system** built using:
- **TF-Luna** (long-range TOF sensor – for main distance)
- **VL53L1X** (short-range TOF sensor – second layer scan)
- **ESP32** (angle tracking + sensor fusion + sending yaw/pitch)
- **MPU6050** (gyroscope + accelerometer for real yaw/pitch tracking)
- **Stepper motor + 555 Timer + CD4017** (automatic rotation, no stepper code)
- **IR sensor** (zero-position reset)
- **Python + Open3D** for real-time 3D pointcloud visualization

The system scans **360° horizontally** and **~30° vertically**, creating a simple 3D point cloud.

---

## 📸 System Overview

### ✔ ESP32 does:
- Reads **VL53L1X distance**
- Reads **Real Yaw + Real Pitch** from **MPU6050**
- Sends data wirelessly to the PC:

### ✔ Arduino UNO does:
- Reads **TF-Luna distance** via UART
- Sends distance to Python via USB serial

### ✔ Stepper Motor Section:
- Controlled by **555 timer → CD4017** (NOT controlled by code)
- ESP32 only *tracks* angle, does not *drive* the motor

### ✔ PC (Python):
- Reads:
- ESP32 (via HTTP GET)
- TF-Luna (via serial)
- Converts spherical → Cartesian coordinates
- Displays the 3D point cloud using **Open3D**

---

## 📡 Data Format

### From ESP32 WiFi:
Example string received:


