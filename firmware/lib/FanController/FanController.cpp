/**
 * @file    FanController.cpp
 * @brief   Fan relay driver implementation.
 * @author  Mahmoud Ali
 * @date    2026
 * @license MIT
 */

#include "FanController.h"
#include "BluetoothManager.h"
#include "SystemState.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

FanController::FanController(QueueHandle_t tempQueue)
    : _tempQueue(tempQueue) {}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

void FanController::begin() {
    pinMode(PIN_FAN_RELAY, OUTPUT);
    // Relay is active-LOW: HIGH = relay open = fan OFF
    digitalWrite(PIN_FAN_RELAY, HIGH);
    _isOn = false;
}

void FanController::turnOn() {
    if (!_isOn) {
        digitalWrite(PIN_FAN_RELAY, LOW);
        _isOn = true;
        SharedState::update([](SystemState& s) { s.fanActive = true; });
        BluetoothManager::instance().send("Fan on?");
        Serial.println(F("[FanController] Fan ON"));
    }
}

void FanController::turnOff() {
    if (_isOn) {
        digitalWrite(PIN_FAN_RELAY, HIGH);
        _isOn = false;
        SharedState::update([](SystemState& s) { s.fanActive = false; });
        BluetoothManager::instance().send("Fan off?");
        Serial.println(F("[FanController] Fan OFF"));
    }
}

bool FanController::isOn() const {
    return _isOn;
}

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS auto task
// ─────────────────────────────────────────────────────────────────────────────

/*static*/ void FanController::autoTaskEntry(void* param) {
    static_cast<FanController*>(param)->autoRun();
}

void FanController::autoRun() {
    int tempValue = 0;
    while (true) {
        // Block indefinitely until a new temperature arrives
        if (xQueueReceive(_tempQueue, &tempValue, portMAX_DELAY) == pdTRUE) {
            if (tempValue >= TEMP_FAN_ON_THRESHOLD) {
                turnOn();
            } else {
                turnOff();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(PERIOD_AUTO_FAN_MS));
    }
}
