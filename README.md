| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# PunchTrack

**PunchTrack** is a compact, wrist-worn punch tracking device designed to measure and stream live motion data for real-time punch classification. Built around the **ESP32-S3 Zero**, it utilizes the **LSM6DS3 IMU** for motion sensing and employs wireless UDP communication to transmit data to a computer for machine learning inference.

## 🔧 Hardware Components

- **ESP32-S3 Zero** – Main MCU with Wi-Fi support
- **LSM6DS3** – 6-axis IMU (3-axis accelerometer + 3-axis gyroscope)
- **3.65V LiPo Battery** – Rechargeable power source
- **TPS61023 Mini Boost Converter** – Boosts battery voltage for stable operation
- **TP4056 USB Charging Module** – Allows safe USB-based battery charging

## 🧠 Firmware & Functionality

- Developed using **ESP-IDF** in the **Espressif IDE**
- IMU data is fetched via **I2C** using burst reads for efficient streaming
- Data is enqueued and transmitted over **UDP sockets** to a host computer
- Python on the host side classifies the data using a **trained Random Forest model**

### Key Concepts Learned

- I2C communication and device addressing
- FreeRTOS tasks, queues, and real-time data handling
- Burst reads (multi-byte sensor data fetches)
- UDP socket programming with ESP32
- Python scripting for network data parsing
- Real-time ML classification (Random Forest inference)

## 📈 Future Plans

- Replace I2C with **SPI** for faster data transfer
- Design and fabricate a custom **PCB** for compact integration
- Improve model performance with additional data and features
- Add OLED or LED display for onboard feedback and stats

## 📷 Concept Design
![Screenshot 2025-06-23 145454](https://github.com/user-attachments/assets/f1677878-8b76-4ea0-b57d-ae7d0895d832)

##🧐 ML Model

- Training using real motion data labelled by punch types
- Random Forest classifier used for initial prototyping
- Future exploration may include neural networks for improved accuracy
