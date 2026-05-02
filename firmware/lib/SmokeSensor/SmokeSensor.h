/**
 * @file    SmokeSensor.h
 * @brief   MQ-2 smoke / gas sensor driver with alert outputs.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Config.h"

/**
 * @class SmokeSensor
 * @brief Reads the MQ-2 analogue output and activates buzzer + LED on alarm.
 *
 * When the ADC value exceeds SMOKE_ALARM_THRESHOLD the sensor
 *  - energises the smoke buzzer
 *  - lights the smoke LED
 *  - updates SharedState::smokeDetected
 *  - sends a Bluetooth notification
 */
class SmokeSensor {
public:
    SmokeSensor() = default;

    /// Configure output pins and set safe initial state.
    void begin();

    /// Read sensor once and apply alarm logic.
    void update();

    /// @return true when smoke/gas threshold is currently exceeded.
    bool isAlarmActive() const;

    /// FreeRTOS task entry. Pass pointer to SmokeSensor instance.
    static void taskEntry(void* param);

private:
    bool _alarmActive = false;
    void run();
};
