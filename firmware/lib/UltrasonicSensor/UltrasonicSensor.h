/**
 * @file    UltrasonicSensor.h
 * @brief   HC-SR04 ultrasonic proximity sensor driver.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <Arduino.h>
#include "Config.h"

/**
 * @class UltrasonicSensor
 * @brief Drives the HC-SR04 via trigger/echo and detects close-range presence.
 *
 * Runs continuously in a FreeRTOS task.
 *
 * When an object is within ULTRASONIC_RANGE_CM:
 *  - The presence LED is lit.
 *  - SharedState::presenceDetected is set to true.
 *  - A Bluetooth notification is sent.
 */
class UltrasonicSensor {
public:
    UltrasonicSensor() = default;

    /// Configure trigger/echo pins.
    void begin();

    /// @return Most recent distance measurement in centimetres.
    int getDistanceCm() const;

    /// @return true when an object is within the detection range.
    bool isPresenceDetected() const;

    /**
     * @brief FreeRTOS task entry point.
     *
     * Loops continuously to perform a measurement.
     */
    static void taskEntry(void* pvParameters);

private:
    int  _distanceCm       = 0;
    bool _presenceDetected = false;

    void measure();
};
