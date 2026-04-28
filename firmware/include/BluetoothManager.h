/**
 * @file    BluetoothManager.h
 * @brief   ESP32 Classic Bluetooth Serial abstraction layer.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <BluetoothSerial.h>
#include "Config.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled — run menuconfig and enable it."
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
#error "Serial Bluetooth (SPP) is not available on this chip/config."
#endif

/**
 * @class BluetoothManager
 * @brief Singleton wrapper around BluetoothSerial.
 *
 * Provides a minimal, named API so the rest of the codebase never touches
 * the raw BluetoothSerial object.  Uses the Meyers-singleton pattern so
 * there is exactly one instance regardless of translation unit.
 *
 * ### Bluetooth Protocol
 * All messages sent to the mobile app end with the `?` delimiter so the
 * app parser can split a stream that may contain multiple concatenated frames.
 *
 * | Message            | Meaning                        |
 * |--------------------|--------------------------------|
 * | `#<value>?`        | Temperature reading (°C)       |
 * | `Fan on?`          | Fan relay energised            |
 * | `Fan off?`         | Fan relay de-energised         |
 * | `Bulb on?`         | Light relay energised          |
 * | `Bulb off?`        | Light relay de-energised       |
 * | `Smoke active?`    | Smoke / gas alarm triggered    |
 * | `Smoke inactive?`  | Smoke / gas alarm cleared      |
 * | `Touch active?`    | Touch intrusion detected       |
 * | `Touch inactive?`  | Touch alarm cleared            |
 * | `Ultrasonic active?`   | Presence within range      |
 * | `Ultrasonic inactive?` | No presence detected       |
 *
 * ### Incoming commands (app → ESP32)
 * | Char | Action                                      |
 * |------|---------------------------------------------|
 * | `A`  | Switch to automatic mode                    |
 * | `M`  | Switch to manual mode                       |
 * | `O`  | Off mode — suspend auto tasks, relays off   |
 * | `F`  | Fan ON (manual)                             |
 * | `Y`  | Fan OFF (manual)                            |
 * | `L`  | Light ON (manual)                           |
 * | `Z`  | Light OFF (manual)                          |
 * | `T`  | Clear touch alarm                           |
 */
class BluetoothManager {
public:
    /// Access the single instance.
    static BluetoothManager& instance();

    /// Start Bluetooth with the device name defined in Config.h.
    void begin();

    /// Send a plain string ending with the `?` delimiter.
    void send(const char* msg);

    /// Send temperature reading in the `#<value>?` frame format.
    void sendTemperature(int tempC);

    /// @return true if at least one byte is available to read.
    bool available();

    /// Read one character from the BT stream.
    char read();

private:
    BluetoothManager() = default;
    BluetoothSerial _serial;
};
