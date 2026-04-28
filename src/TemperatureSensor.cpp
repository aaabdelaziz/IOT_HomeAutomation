/**
 * @file    TemperatureSensor.cpp
 * @brief   DHT11 temperature sensor driver implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "TemperatureSensor.h"
#include "BluetoothManager.h"
#include "SystemState.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

TemperatureSensor::TemperatureSensor(QueueHandle_t tempQueue,
                                     QueueHandle_t displayQueue)
    : _dht(PIN_DHT, DHT_TYPE),
      _tempQueue(tempQueue),
      _displayQueue(displayQueue) {}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

void TemperatureSensor::begin() {
    _dht.begin();
}

int TemperatureSensor::readTemperature() {
    int t = static_cast<int>(_dht.readTemperature());
    if (isnan(_dht.readTemperature())) {
        Serial.println(F("[TemperatureSensor] ERROR: Failed to read DHT11"));
        return -127; // sentinel value for error
    }
    return t;
}

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS task
// ─────────────────────────────────────────────────────────────────────────────

/*static*/ void TemperatureSensor::taskEntry(void* param) {
    static_cast<TemperatureSensor*>(param)->run();
}

void TemperatureSensor::run() {
    while (true) {
        int t = static_cast<int>(_dht.readTemperature());

        if (isnan(static_cast<float>(t))) {
            Serial.println(F("[TemperatureSensor] Sensor read failed — skipping"));
            vTaskDelay(pdMS_TO_TICKS(PERIOD_TEMP_READ_MS));
            continue;
        }

        Serial.printf("[TemperatureSensor] Temperature: %d°C\n", t);

        // Update shared state
        SharedState::update([t](SystemState& s) { s.temperature = t; });

        // Transmit via Bluetooth
        BluetoothManager::instance().sendTemperature(t);

        // Feed both consumer queues (non-blocking — discard if full)
        xQueueSend(_tempQueue,    &t, pdMS_TO_TICKS(10));
        xQueueSend(_displayQueue, &t, pdMS_TO_TICKS(10));

        vTaskDelay(pdMS_TO_TICKS(PERIOD_TEMP_READ_MS));
    }
}
