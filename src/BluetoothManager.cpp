/**
 * @file    BluetoothManager.cpp
 * @brief   ESP32 Classic Bluetooth Serial abstraction — implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "BluetoothManager.h"
#include <Arduino.h>

// ─────────────────────────────────────────────────────────────────────────────
// Singleton access
// ─────────────────────────────────────────────────────────────────────────────

/*static*/ BluetoothManager& BluetoothManager::instance() {
    static BluetoothManager inst;
    return inst;
}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

void BluetoothManager::begin() {
    _serial.begin(BT_DEVICE_NAME);
    Serial.printf("[BluetoothManager] Started — device name: \"%s\"\n",
                  BT_DEVICE_NAME);
    Serial.println(F("[BluetoothManager] Pair this device from your mobile app."));
}

void BluetoothManager::send(const char* msg) {
    if (_serial.hasClient()) {
        _serial.print(msg);
    }
}

void BluetoothManager::sendTemperature(int tempC) {
    if (_serial.hasClient()) {
        _serial.print('#');
        _serial.print(tempC);
        _serial.print('?');
    }
}

bool BluetoothManager::available() {
    return _serial.available() > 0;
}

char BluetoothManager::read() {
    return static_cast<char>(_serial.read());
}
