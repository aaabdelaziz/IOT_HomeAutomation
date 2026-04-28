# Communication Protocols

The ESP32 Smart Home hub communicates with the outside world (like a companion mobile app) using **Bluetooth Classic Serial Port Profile (SPP)**. 

This enables simple, robust, two-way text communication without requiring complex network setups, WiFi provisioning, or external MQTT brokers.

## Bluetooth Device Identity

- **Device Name:** `SmartHome-ESP32`
- **Protocol:** Bluetooth Classic (SPP)
- **Pairing:** Standard PIN pairing (usually `1234` or `0000` depending on the device)

## Message Format

Because SPP is a continuous stream of data, the protocol requires a delimiter so the receiving application knows when a message ends. 

**All messages sent by the ESP32 end with a `?` character.**

## 1. Outgoing Notifications (ESP32 → Mobile App)

The `BluetoothManager` task continually monitors the `SharedState`. When it detects a change in the state of an actuator or an alarm, it immediately pushes a string notification over Bluetooth.

| Event | String Transmitted |
| :--- | :--- |
| **Temperature Update** | `#<value>?` *(e.g., `#24?`)* |
| **Fan Relay turned ON** | `Fan on?` |
| **Fan Relay turned OFF** | `Fan off?` |
| **Light Relay turned ON** | `Bulb on?` |
| **Light Relay turned OFF** | `Bulb off?` |
| **Smoke Gas detected** | `Smoke active?` |
| **Smoke Gas cleared** | `Smoke inactive?` |
| **Capacitive Touch detected**| `Touch active?` |
| **Capacitive Touch cleared** | `Touch inactive?` |
| **Object in presence zone** | `Ultrasonic active?` |
| **Object left presence zone**| `Ultrasonic inactive?` |

## 2. Incoming Commands (Mobile App → ESP32)

The companion mobile app controls the ESP32 by sending single-character string commands over the Bluetooth connection. These commands are parsed by the `SwitchController` task.

### Mode Switching Commands

| Character | Command | Action Performed by ESP32 |
| :---: | :--- | :--- |
| **`A`** | **Automatic Mode** | Resumes the `AutoFan` and `AutoLight` FreeRTOS tasks. The system will now autonomously control the relays based on the DHT11 and LDR sensor readings. |
| **`M`** | **Manual Mode** | Suspends the `AutoFan` and `AutoLight` tasks. Actuators will only change state when explicitly commanded via Bluetooth. |
| **`O`** | **Off Mode** | Suspends the auto tasks AND immediately de-energizes both the Fan and Light relays. |

> [!IMPORTANT]
> When the ESP32 boots up, it starts in **Manual Mode** by default.

### Actuator Control Commands

*These commands are typically only effective when the system is in Manual Mode.*

| Character | Command | Action Performed by ESP32 |
| :---: | :--- | :--- |
| **`F`** | Fan ON | Energizes the Fan relay (GPIO 17) |
| **`Y`** | Fan OFF | De-energizes the Fan relay |
| **`L`** | Light ON | Energizes the Light bulb relay (GPIO 16) |
| **`Z`** | Light OFF | De-energizes the Light bulb relay |
| **`T`** | Clear Touch | Acknowledges and silences the capacitive touch intrusion alarm |
