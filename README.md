# Air Quality Monitoring System
![3882565164e0eabeb3f1](https://github.com/user-attachments/assets/9f7a8be7-b07e-4377-9b47-6b08ac48dce9)

## Overview
This project is an intelligent IoT node designed to monitor real-time environmental parameters, including **PM2.5, PM10, CO2, gas concentration, temperature, and humidity**. Engineered for high availability, the system integrates **Blynk Cloud**, a **Local Web Server**, and **Google Sheets API** to provide a comprehensive monitoring experience even during network instability.

## Key Features
* **Real-Time Multi-Cloud Monitoring**: Simultaneously pushes data to Blynk for mobile alerts and Google Sheets for long-term historical logging.
* **Batch Data Processing**: Implemented a buffer strategy to **batch 12 samples per minute**, significantly optimizing network bandwidth and reducing API call frequency.
* **Advanced Signal Filtering**: Applied a **Simple Moving Average (SMA) filter** to reduce sensor noise and ensure data stability for PM2.5 and PM10 measurements.
* **Precise Timestamping**: Synchronized system time via **NTP (Network Time Protocol)** to ensure every data log entry is accurately timestamped.
* **Intelligent Anti-Spam Alerts**: Features a state-machine logic to send only a single notification when air quality crosses a danger threshold, preventing notification fatigue.

<img width="521" height="791" alt="622379860_1566266061248160_175080920250572696_n" src="https://github.com/user-attachments/assets/9af99dc2-2518-424a-8933-82192144ea4d" />

![ebb95770-63ad-43c2-9f0e-61bad42a665d](https://github.com/user-attachments/assets/7c7373b5-862d-4e96-83b3-19f8346c986c)

![cbf13f2f-ea43-4f09-b93a-094669a6b34d](https://github.com/user-attachments/assets/5fe5b4a9-0973-4bf3-8cff-e61cf96fe844)
## Components
* **ESP32-WROOM-32**: The central processing unit managing Wi-Fi connectivity and data processing.
* **GP2Y1010AU0F**: Optical dust sensor for measuring PM2.5 and PM10 concentrations.
* **MQ135**: Gas sensor used for monitoring air quality and detecting harmful gases.
* **DHT11**: Digital sensor for real-time temperature and humidity tracking.


---

## Getting Started

To get started with this project, you’ll need the following:

1. An **ESP32** development board and the sensors listed above.
2. **Arduino IDE** with the ESP32 core and Blynk/DHT libraries installed.
3. Setup a **Google Apps Script** to receive and log HTTPS POST requests from the ESP32.


## Future Potential
This project is just the beginning. There’s room for numerous improvements and enhancements, such as:
* **Autonomous Navigation**: Integrating the sensor node with a mobile platform for spatial air quality mapping.
* **Sensor Integration**: Upgrading to laser-based PM2.5 sensors for higher precision.
* **UI Refinement**: Developing a more interactive and data-rich local web dashboard.

I welcome contributions from anyone interested in taking this project further. Whether you’re looking to add features, optimize the code, or propose new ideas, your input is valued.
