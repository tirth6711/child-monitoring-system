# Smart Wearable Child Monitoring System

A GPS and LoRa based wearable device that tracks a child's real time location and alerts parents when the child moves beyond a safe distance. The system works without a SIM card or internet connection on the transmitter side, making it low cost and easy to deploy.

## Motivation

Child safety is a growing concern, and traditional monitoring methods often fall short in real time situations. This project builds a lightweight, cost effective wearable device that sends live location updates and safety alerts to a parent's mobile device through a web dashboard.

## Overview

The system has two main units:

- **Transmitter unit**: A wearable module carried by the child. It acquires GPS coordinates and sends them to the receiver over LoRa.
- **Receiver unit**: Stays with the parent. It receives the location data, calculates the distance to the child using the Haversine formula, and displays everything on a web based map interface.

When the child crosses a predefined safe distance, the receiver triggers a vibration alert to notify the parent.

## Features

- Real time GPS location tracking
- Long range communication using LoRa, no SIM card or internet required on the transmitter
- Web based dashboard for live location and path visualization
- Configurable geofence and distance alerts
- Vibration alert on the parent side when the child exceeds the safe boundary
- Low power design suitable for battery operated wearables

## Hardware Components

Component:

ESP32 Central microcontroller, handles GPS data, LoRa communication, and hosts the web server |
| NEO-6MV2 GPS Module | Provides latitude, longitude, and time data |
| LoRa SX1278 (Ra-02) | Long range wireless transmission of GPS data |
| Li-ion battery (3.7V) | Power source for the wearable transmitter |

## System Architecture

### GPS Module (UART)


ESP32 GPIO16  -> GPS TX
ESP32 GPIO17  -> GPS RX
Protocol: UART (Serial2)
Baud rate: 9600 bps


### LoRa Module (SPI)


ESP32 GPIO5   -> LoRa NSS (Chip Select)
ESP32 GPIO18  -> LoRa SCK (Clock)
ESP32 GPIO19  -> LoRa MISO
ESP32 GPIO23  -> LoRa MOSI
ESP32 GPIO14  -> LoRa RESET
ESP32 GPIO26  -> LoRa DIO0 (Interrupt Pin)


### Power Supply

ESP32 VIN  -> 5V regulated supply
ESP32 3.3V -> Sensor module supply
ESP32 GND  -> Common system ground


### Optional Peripheral Pins (Future Expansion)


GPIO27 -> Buzzer (distance alert)
GPIO25 -> Vibration motor (wearable feedback)
GPIO34 -> Battery voltage sensing (ADC input)


## How It Works

1. The GPS module on the transmitter acquires the child's coordinates over UART.
2. The ESP32 encodes this data and sends it through the LoRa module using SPI.
3. The receiver unit picks up the packet over LoRa and decodes the coordinates.
4. The receiver ESP32 calculates the distance between the parent and child using the Haversine formula.
5. Location data and distance are shown on a live web dashboard hosted by the ESP32's WiFi module.
6. If the distance exceeds the configured safe boundary, a vibration alert is triggered on the parent side.

Data is transmitted at a configurable interval, typically every 1 to 5 seconds, balancing tracking accuracy against battery life.

## Working Conditions and Limitations

- The GPS module needs a clear line of sight to satellites. Indoor use, tall buildings, and dense urban areas can delay or block position updates.
- LoRa communication works reliably up to a few kilometers under line of sight conditions, but the practical tested range for this system is around 200 meters, since obstacles like buildings and trees reduce signal strength.
- The web dashboard depends on a stable WiFi connection at the parent's end. LoRa reception itself does not require internet, but the live map display does.
- Operating temperature range is 0°C to 50°C, standard for consumer electronics.
- Cold start GPS lock can take 30 to 60 seconds. Warm and hot starts are much faster.

## Technical Concepts Used

- **GPS positioning**: satellite trilateration using time stamped signals from multiple satellites, giving coordinates in the WGS-84 reference frame.
- **LoRa modulation**: Chirp Spread Spectrum technique that trades data rate for range and noise resilience, tuned through spreading factor, bandwidth, and coding rate.
- **Haversine formula**: calculates the great circle distance between two latitude and longitude points, used here to measure the distance between parent and child.
- **UART**: asynchronous serial protocol used for GPS to ESP32 communication.
- **SPI**: synchronous, full duplex protocol used for ESP32 to LoRa communication.
- **Embedded web server**: hosted directly on the ESP32 using its WiFi stack, serving the live tracking dashboard to the parent's browser.

## Future Scope

- Elderly tracking
- Pet tracking systems
- Asset tracking
- Personal safety devices
- Health monitoring integration
- Cloud storage for location history

## Conclusion

This project delivers a simple, reliable, and low cost solution for child safety using GPS and LoRa technology. By avoiding GSM and SIM card dependency, it removes recurring costs while still providing near real time tracking, live map visualization, and distance based alerts. The design is modular enough to extend into elderly care, pet tracking, and general asset monitoring applications.

## References

1. Child Safety Device for Location Tracking, International Journal of Scientific Research and Engineering Development, Volume 8 Issue 6, Nov to Dec 2025
2. A Research on Child Safety Wearable Devices, IEEE ICSCAN, 2019
3. An Easy to use Low cost Child Tracking System Based on IoT, IEEE ICOT, 2019
4. u-blox AG, GPS Modules Data Sheet, Switzerland, 2019
5. Semtech Corporation, LoRa Modulation Basics Application Note AN1200.22, USA, 2015
6. R. W. Sinnott, Virtues of the Haversine, Sky and Telescope Magazine, 1984
7. Kaplan and Hegarty, Understanding GPS, Artech House
