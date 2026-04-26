/**
 * @file    LightSensor.h
 * @brief   LDR (Light-Dependent Resistor) analogue sensor driver.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Config.h"

/**
 * @class LightSensor
 * @brief Reads ambient light intensity from an LDR via the ESP32 ADC.
 *
 * Enqueues the raw ADC value (0–4095) to `lightQueue` for the
 * LightController auto task to consume.
 */
class LightSensor {
public:
    explicit LightSensor(QueueHandle_t lightQueue);

    /// Pin is analogue — no explicit begin() needed, but kept for symmetry.
    void begin();

    /// @return Raw ADC reading (0–4095). Higher = brighter ambient light.
    int readLightLevel();

    /// FreeRTOS task entry. Pass pointer to LightSensor instance.
    static void taskEntry(void* param);

private:
    QueueHandle_t _lightQueue;
    void run();
};
