import serial
import matplotlib.pyplot as plt
import math

# Change COM port for Windows (e.g., "COM3") or "/dev/ttyUSB0" for Linux
ser = serial.Serial("COM3", 115200)

plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111, polar=True)

angles = []
distances = []

while True:
    try:
        line = ser.readline().decode().strip()
        if "," in line:
            angle, dist = map(float, line.split(","))
            rad = math.radians(angle % 360)

            angles.append(rad)
            distances.append(dist)

            ax.clear()
            ax.set_theta_zero_location("N")
            ax.set_theta_direction(-1)
            ax.scatter(angles, distances, c="blue", s=10)
            plt.draw()
            plt.pause(0.01)
    except KeyboardInterrupt:
        print("Stopped by user")
        break
