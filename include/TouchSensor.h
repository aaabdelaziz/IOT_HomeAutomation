/**
 * @file    TouchSensor.h
 * @brief   ESP32 capacitive-touch intrusion detector with alert outputs.
 * @author  Mahmoud Ali
 * @date    2026
 * @license MIT
 */

#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Config.h"

/**
 * @class TouchSensor
 * @brief Reads the ESP32 capacitive touch pad and triggers a security alert.
 *
 * When the raw touch value drops below TOUCH_ALARM_THRESHOLD the sensor
 *  - lights the touch LED
 *  - activates the touch buzzer
 *  - updates SharedState::touchDetected
 *  - sends a Bluetooth notification
 *
 * The alarm can be silenced externally (e.g. via SwitchController) by
 * calling `clearAlarm()`.
 */
class TouchSensor {
public:
    TouchSensor() = default;

    /// Configure output pins.
    void begin();

    /// Silence the buzzer and LED — called from SwitchController on 'T' command.
    void clearAlarm();

    /// @return true when a touch event is currently latched.
    bool isAlarmActive() const;

    /// FreeRTOS task entry. Pass pointer to TouchSensor instance.
    static void taskEntry(void* param);

private:
    bool _alarmActive = false;
    void run();
};
