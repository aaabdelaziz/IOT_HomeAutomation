/**
 * @file    LightController.cpp
 * @brief   Bulb relay driver implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "LightController.h"
#include "BluetoothManager.h"
#include "SystemState.h"

LightController::LightController(QueueHandle_t lightQueue)
    : _lightQueue(lightQueue) {}

void LightController::begin() {
    pinMode(PIN_LIGHT_RELAY, OUTPUT);
    digitalWrite(PIN_LIGHT_RELAY, HIGH); // active-LOW relay: HIGH = OFF
    _isOn = false;
}

void LightController::turnOn() {
    if (!_isOn) {
        digitalWrite(PIN_LIGHT_RELAY, LOW);
        _isOn = true;
        SharedState::update([](SystemState& s) { s.lightActive = true; });
        BluetoothManager::instance().send("Bulb on?");
        Serial.println(F("[LightController] Bulb ON"));
    }
}

void LightController::turnOff() {
    if (_isOn) {
        digitalWrite(PIN_LIGHT_RELAY, HIGH);
        _isOn = false;
        SharedState::update([](SystemState& s) { s.lightActive = false; });
        BluetoothManager::instance().send("Bulb off?");
        Serial.println(F("[LightController] Bulb OFF"));
    }
}

bool LightController::isOn() const {
    return _isOn;
}

/*static*/ void LightController::autoTaskEntry(void* param) {
    static_cast<LightController*>(param)->autoRun();
}

void LightController::autoRun() {
    int lightValue = 0;
    while (true) {
        if (xQueueReceive(_lightQueue, &lightValue, portMAX_DELAY) == pdTRUE) {
            // LDR: high ADC value = bright room → bulb not needed
            // Low ADC value = dark room → turn bulb on
            if (lightValue >= LIGHT_BULB_ON_THRESHOLD) {
                turnOff();
            } else {
                turnOn();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(PERIOD_AUTO_LIGHT_MS));
    }
}
