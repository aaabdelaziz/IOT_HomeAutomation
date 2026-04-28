/**
 * @file    TemperatureSensor.h
 * @brief   DHT11 temperature sensor driver interface.
 *
 * Wraps the Adafruit DHT library in a clean C++ class with error-handling
 * and a FreeRTOS task entry point.
 *
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <DHT.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Config.h"

/**
 * @class TemperatureSensor
 * @brief Reads ambient temperature from a DHT11 sensor and enqueues results.
 *
 * ### Responsibilities
 * - Initialises the DHT library.
 * - Exposes a static FreeRTOS task function (`taskEntry`) that continuously
 *   reads the sensor and sends values to two queues:
 *     - `tempQueue`  — consumed by the FanController auto-mode task.
 *     - `displayQueue` — consumed by the OLEDDisplay task.
 * - Transmits the reading over Bluetooth using the shared BluetoothManager.
 */
class TemperatureSensor {
public:
    /**
     * @brief Construct with injected queue handles.
     * @param tempQueue    Queue for FanController (depth = QUEUE_TEMP_DEPTH).
     * @param displayQueue Queue for OLEDDisplay (depth = QUEUE_TEMP_DEPTH).
     */
    explicit TemperatureSensor(QueueHandle_t tempQueue,
                               QueueHandle_t displayQueue);

    /// Initialise DHT peripheral — call once in setup().
    void begin();

    /**
     * @brief Read temperature once.
     * @return Temperature in °C, or -127 on sensor fau+lt.
     */
    int readTemperature();

    /**
     * @brief FreeRTOS task entry point.
     *
     * Signature matches `TaskFunction_t`.  Pass a pointer to a
     * TemperatureSensor instance as the parameter.
     */
    static void taskEntry(void* param);

private:
    DHT           _dht;
    QueueHandle_t _tempQueue;
    QueueHandle_t _displayQueue;

    void run(); ///< Internal task loop
};
