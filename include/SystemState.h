/**
 * @file    SystemState.h
 * @brief   Shared, thread-safe system-state structure.
 *
 * All FreeRTOS tasks communicate status through this singleton-like struct.
 * Access is protected by a FreeRTOS mutex, so no race conditions occur between
 * the sensor tasks (writers) and the display / BT tasks (readers).
 *
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * @brief Snapshot of every subsystem state.
 *
 * Keep fields small (bool / int) so that copying under the mutex is cheap.
 */
struct SystemState {
    bool fanActive        = false; ///< true → fan relay energised
    bool lightActive      = false; ///< true → bulb relay energised
    bool smokeDetected    = false; ///< true → MQ-2 threshold exceeded
    bool touchDetected    = false; ///< true → capacitive touch active
    bool presenceDetected = false; ///< true → object within range
    int  temperature      = 0;     ///< Latest DHT11 reading (°C)
    int  lightLevel       = 0;     ///< Latest LDR ADC reading
    int  smokeLevel       = 0;     ///< Latest MQ-2 ADC reading
    int  distanceCm       = 0;     ///< Latest ultrasonic reading (cm)
};

/**
 * @brief Thread-safe wrapper around SystemState.
 *
 * Usage (writer):
 * @code
 *   SharedState::update([](SystemState& s){ s.fanActive = true; });
 * @endcode
 *
 * Usage (reader):
 * @code
 *   SystemState snap = SharedState::snapshot();
 * @endcode
 */
class SharedState {
public:
    /// Initialise the mutex — call once before any task starts.
    static void init() {
        _mutex = xSemaphoreCreateMutex();
    }

    /// Apply a mutation functor under the mutex.
    template <typename Fn>
    static void update(Fn fn) {
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            fn(_state);
            xSemaphoreGive(_mutex);
        }
    }

    /// Return an atomic copy of the current state.
    static SystemState snapshot() {
        SystemState copy;
        if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            copy = _state;
            xSemaphoreGive(_mutex);
        }
        return copy;
    }

private:
    static SystemState      _state;
    static SemaphoreHandle_t _mutex;
};

// Definitions moved to src/SystemState.cpp
