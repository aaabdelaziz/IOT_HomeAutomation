/**
 * @file    LightSensor.cpp
 * @brief   LDR analogue sensor implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "LightSensor.h"
#include "SystemState.h"

LightSensor::LightSensor(QueueHandle_t lightQueue)
    : _lightQueue(lightQueue) {}

void LightSensor::begin() {
    // Analogue input — no pinMode needed on ESP32 ADC pins.
}

int LightSensor::readLightLevel() {
    return analogRead(PIN_LIGHT_SENSOR);
}

/*static*/ void LightSensor::taskEntry(void* param) {
    static_cast<LightSensor*>(param)->run();
}

void LightSensor::run() {
    while (true) {
        int lv = analogRead(PIN_LIGHT_SENSOR);
        Serial.printf("[LightSensor] Light level: %d\n", lv);

        SharedState::update([lv](SystemState& s) { s.lightLevel = lv; });

        // Non-blocking send — drop reading if queue is full
        xQueueSend(_lightQueue, &lv, pdMS_TO_TICKS(10));

        vTaskDelay(pdMS_TO_TICKS(PERIOD_LIGHT_READ_MS));
    }
}
