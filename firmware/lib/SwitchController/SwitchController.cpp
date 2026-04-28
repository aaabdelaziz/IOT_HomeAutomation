/**
 * @file    SwitchController.cpp
 * @brief   Bluetooth command parser and actuator dispatcher — implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "SwitchController.h"
#include "BluetoothManager.h"
#include "SystemState.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

SwitchController::SwitchController(FanController&   fan,
                                   LightController& light,
                                   TouchSensor&     touch,
                                   TaskHandle_t     autoFanTask,
                                   TaskHandle_t     autoLightTask)
    : _fan(fan),
      _light(light),
      _touch(touch),
      _autoFanTask(autoFanTask),
      _autoLightTask(autoLightTask) {}

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS task
// ─────────────────────────────────────────────────────────────────────────────

/*static*/ void SwitchController::taskEntry(void* param) {
    static_cast<SwitchController*>(param)->run();
}

void SwitchController::run() {
    while (true) {
        if (BluetoothManager::instance().available()) {
            char cmd = BluetoothManager::instance().read();
            Serial.printf("[SwitchController] Command received: '%c'\n", cmd);
            processCommand(cmd);
        }
        // Short yield — keeps this task responsive without burning CPU
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Command dispatch
// ─────────────────────────────────────────────────────────────────────────────

void SwitchController::processCommand(char cmd) {
    switch (cmd) {

    case 'A': // ── Automatic mode ────────────────────────────────────────────
        Serial.println(F("[SwitchController] Mode → AUTO"));
        vTaskResume(_autoFanTask);
        vTaskResume(_autoLightTask);
        break;

    case 'M': // ── Manual mode ───────────────────────────────────────────────
        Serial.println(F("[SwitchController] Mode → MANUAL"));
        vTaskSuspend(_autoFanTask);
        vTaskSuspend(_autoLightTask);
        break;

    case 'O': // ── Off mode ──────────────────────────────────────────────────
        Serial.println(F("[SwitchController] Mode → OFF"));
        vTaskSuspend(_autoFanTask);
        vTaskSuspend(_autoLightTask);
        _fan.turnOff();
        _light.turnOff();
        break;

    case 'F': // ── Fan ON (manual) ───────────────────────────────────────────
        _fan.turnOn();
        break;

    case 'Y': // ── Fan OFF (manual) ──────────────────────────────────────────
        _fan.turnOff();
        break;

    case 'L': // ── Light ON (manual) ─────────────────────────────────────────
        _light.turnOn();
        break;

    case 'Z': // ── Light OFF (manual) ────────────────────────────────────────
        _light.turnOff();
        break;

    case 'T': // ── Clear touch alarm ─────────────────────────────────────────
        _touch.clearAlarm();
        break;

    default:
        Serial.printf("[SwitchController] Unknown command: '%c' — ignored\n", cmd);
        break;
    }
}
