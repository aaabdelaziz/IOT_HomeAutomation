# 📱 Mobile Application

> **Status:** Planned — not yet implemented.

This directory will contain the mobile companion app for the ESP32 Smart Home Automation system.

---

## Planned Stack

| Layer | Technology |
|---|---|
| Framework | Flutter 3 (iOS + Android) |
| Language | Dart |
| Bluetooth | `flutter_bluetooth_serial` package |
| UI | Material Design 3 |

---

## Bluetooth SPP Protocol

The app communicates with the ESP32 over **Bluetooth Classic SPP**.  
The ESP32 advertises as **`SmartHome-ESP32`**.

All messages from the ESP32 end with the `?` delimiter.

### Commands: App → ESP32

| Char | Effect |
|---|---|
| `A` | **Automatic mode** — resume AutoFan + AutoLight tasks |
| `M` | **Manual mode** — suspend AutoFan + AutoLight tasks |
| `O` | **Off mode** — suspend auto tasks, turn both relays off |
| `F` | Fan ON (manual) |
| `Y` | Fan OFF (manual) |
| `L` | Light ON (manual) |
| `Z` | Light OFF (manual) |
| `T` | Clear touch alarm |

### Notifications: ESP32 → App

| Message | Meaning |
|---|---|
| `#<value>?` | Temperature reading in °C |
| `Fan on?` / `Fan off?` | Fan relay state changed |
| `Bulb on?` / `Bulb off?` | Light relay state changed |
| `Smoke active?` / `Smoke inactive?` | Smoke/gas alarm state |
| `Touch active?` / `Touch inactive?` | Intrusion alarm state |
| `Ultrasonic active?` / `Ultrasonic inactive?` | Presence detection state |

---

## Planned Screens

- **Dashboard** — live temperature, fan/light/smoke/touch/presence status cards
- **Control panel** — mode selector (Auto / Manual / Off) + manual relay toggles
- **Alerts** — notification feed for smoke and intrusion events

---

*See the [firmware README](../firmware/src/../../../README.md) for full system documentation.*
