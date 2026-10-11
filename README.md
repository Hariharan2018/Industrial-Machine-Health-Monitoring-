# Industrial Machine Health Monitoring

## Overview
An embedded IoT project designed to monitor machine or environmental parameters using sensors, an STM32 microcontroller, an ESP32 and the ThingSpeak cloud platform.

## Hardware
- STM32 Nucleo development board
- ESP32 development board
- Sensors used in the implemented prototype
- Breadboard and jumper wires

## Technologies
- Embedded C and STM32 HAL
- Arduino IDE and ESP32
- UART/USART communication
- Wi-Fi and ThingSpeak IoT cloud

## Working Principle
1. The STM32 collects sensor readings.
2. Sensor data is transmitted to the ESP32 through UART.
3. The ESP32 connects to Wi-Fi.
4. Sensor readings are uploaded to ThingSpeak.
5. The dashboard displays measurements over time.

## Repository Structure
- `STM32/` — STM32 firmware
- `ESP32/` — ESP32 communication and cloud upload code
- `docs/` — Project report and connection details
- `images/` — Hardware and dashboard screenshots

## Setup
Configure the sensor pins and UART settings according to the hardware. Enter Wi-Fi credentials and a private ThingSpeak Write API Key locally before uploading the ESP32 firmware.

## Results
Add genuine hardware photographs, Serial Monitor output and ThingSpeak dashboard screenshots after testing.
