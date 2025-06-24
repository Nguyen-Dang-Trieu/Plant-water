# 🌱 Smart Plant Monitoring and Watering System
## 📌 Introduction
This is the final project for the Embedded Systems course.
The system monitors environmental parameters for crops and supports automated watering.

## 📖 Project Description
The system consists of sensors that measure environmental conditions such as temperature, humidity, light intensity, and soil moisture.
- Data is collected by an Arduino Uno (ATmega328P) running bare-metal C code (no Arduino libraries).
- The Arduino sends the data to a Raspberry Pi 3B+, which acts as the central processing server.
- The Raspberry Pi publishes the data via MQTT protocol to a laptop (host) for real-time display and monitoring.
- The Arduino can control a DC water pump motor for automated watering when soil moisture is low.

## 🛠️ Devices
- Raspberry Pi 3B+ (MQTT broker & data processing)
- Arduino Uno (ATmega328P) (bare-metal C code for sensor reading & control)
- Sensors:
  - DHT22 (temperature & humidity)
  - BH1750 (light intensity)
  - Soil moisture sensor
- DC water pump motor

## ⚙️ Implementation
- ✔️ The Arduino Uno is programmed in bare-metal C, directly accessing hardware registers for GPIO, ADC, and I2C communication.
- ✔️ The Raspberry Pi 3B+ runs an MQTT broker (HiveMQ) and C client to process and forward data.
- ✔️ The laptop subscribes to MQTT topics to receive and display the data.

## 📈 Diagram
### System
<img src="./Image/system_.png" width="800">

### Monitor screen
![](./Image/web.png)

## 🚀 How to Run
