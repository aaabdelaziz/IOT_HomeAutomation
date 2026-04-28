/**
 * @file    LightController.h
 * @brief   Bulb relay driver — automatic and manual modes.
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
 * @class LightController
 * @brief Controls a relay-switched light bulb in auto and manual modes.
 *
 * **Auto mode** — monitors `lightQueue`; turns bulb on when ADC value
 * exceeds LIGHT_BULB_ON_THRESHOLD (dark room) and off otherwise.
 *
 * **Manual mode** — use `turnOn()` / `turnOff()` directly.
 */
class LightController {
public:
    explicit LightController(QueueHandle_t lightQueue);

    void begin();

    void turnOn();
    void turnOff();

    bool isOn() const;

    /// FreeRTOS task entry — suspend to enter manual mode.
    static void autoTaskEntry(void* param);

private:
    QueueHandle_t _lightQueue;
    bool          _isOn = false;

    void autoRun();
};
