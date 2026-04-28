# Welcome to the ESP32 Smart Home Automation Wiki!

This Wiki provides detailed documentation for the Smart Home Automation firmware project, built using object-oriented C++ and FreeRTOS on the ESP32 platform.

## What's in this Wiki?

Whether you're looking to wire up the hardware, understand the software architecture, or build a custom mobile app to communicate with the ESP32, you'll find the information here.

### 📚 Table of Contents

*   **[System Architecture](System-Architecture.md)**
    *   Learn how the firmware uses FreeRTOS tasks, thread-safe queues, and a mutex-protected Singleton (`SharedState`) to manage multiple sensors concurrently without blocking.
    *   Understand the new PlatformIO `lib/` component-based structure.
*   **[Hardware Wiring Guide](Hardware-Wiring.md)**
    *   A comprehensive pinout reference for wiring the DHT11, MQ-2, LDR, HC-SR04, capacitive touch pad, OLED, and relays to the ESP32.
*   **[Communication Protocols](Communication-Protocols.md)**
    *   Details on the Bluetooth Classic Serial Port Profile (SPP) implementation.
    *   Learn the exact string commands to send from a mobile app and the format of the telemetry data the ESP32 broadcasts back.

## Project Overview

This system transforms a standard ESP32 into a fully local, cloud-free home automation hub. 

**Core capabilities:**
*   **Auto Mode:** The system autonomously triggers actuators based on sensor readings (e.g., turning on the fan when the temperature exceeds 33°C, or turning on lights when the room goes dark).
*   **Manual Mode:** Auto tasks are suspended, giving full control over the relays to a connected Bluetooth device.
*   **Security & Alerts:** Smoke, gas, and capacitive touch intrusions instantly trigger local buzzers and broadcast Bluetooth alerts regardless of the active mode.
*   **Live Dashboard:** A localized SSD1306 OLED displays temperature and active system states at a 5Hz refresh rate.
