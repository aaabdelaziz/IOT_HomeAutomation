/**
 * @file    FanController.h
 * @brief   Fan relay driver — automatic and manual modes.
 * @author  Mahmoud Ali
 * @date    2026
 * @license MIT
 */

#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Config.h"

/**
 * @class FanController
 * @brief Controls a relay-switched fan in both automatic and manual modes.
 *
 * **Automatic mode** — a suspended FreeRTOS task (`autoTaskEntry`) watches
 * the temperature queue and turns the relay on/off based on the threshold
 * defined in Config.h.
 *
 * **Manual mode** — call `turnOn()` / `turnOff()` directly from the
 * SwitchController task; the auto task must be suspended first.
 */
class FanController {
public:
    /**
     * @param tempQueue  Queue populated by TemperatureSensor (read-only consumer).
     */
    explicit FanController(QueueHandle_t tempQueue);

    /// Configure relay GPIO and set initial state (relay off).
    void begin();

    /// Energise the relay → fan on.
    void turnOn();

    /// De-energise the relay → fan off.
    void turnOff();

    /// @return true if the fan relay is currently energised.
    bool isOn() const;

    /**
     * @brief FreeRTOS task entry — run in auto mode only.
     * Suspend this task to switch to manual mode.
     */
    static void autoTaskEntry(void* param);

private:
    QueueHandle_t _tempQueue;
    bool          _isOn = false;

    void autoRun();
};
