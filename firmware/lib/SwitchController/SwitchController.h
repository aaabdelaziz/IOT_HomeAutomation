/**
 * @file    SwitchController.h
 * @brief   Bluetooth command parser — manual/auto/off mode switching.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Config.h"
#include "FanController.h"
#include "LightController.h"
#include "TouchSensor.h"

/**
 * @class SwitchController
 * @brief Reads Bluetooth commands and dispatches them to actuator objects.
 *
 * Runs as a dedicated FreeRTOS task.  Maintains handles to the auto-mode
 * tasks so it can suspend / resume them on mode changes.
 *
 * ### Command table
 * | Char | Effect                                                  |
 * |------|---------------------------------------------------------|
 * | `A`  | Resume autoFan + autoLight tasks (automatic mode)       |
 * | `M`  | Suspend autoFan + autoLight tasks (manual mode)         |
 * | `O`  | Suspend auto tasks, turn both relays off (off mode)     |
 * | `F`  | Fan ON  (manual)                                        |
 * | `Y`  | Fan OFF (manual)                                        |
 * | `L`  | Light ON  (manual)                                      |
 * | `Z`  | Light OFF (manual)                                      |
 * | `T`  | Clear touch alarm                                       |
 */
class SwitchController {
public:
    /**
     * @param fan          Reference to the FanController instance.
     * @param light        Reference to the LightController instance.
     * @param touch        Reference to the TouchSensor instance.
     * @param autoFanTask  Handle to the auto-fan FreeRTOS task.
     * @param autoLightTask Handle to the auto-light FreeRTOS task.
     */
    SwitchController(FanController&   fan,
                     LightController& light,
                     TouchSensor&     touch,
                     TaskHandle_t     autoFanTask,
                     TaskHandle_t     autoLightTask);

    /// FreeRTOS task entry. Pass pointer to SwitchController instance.
    static void taskEntry(void* param);

private:
    FanController&   _fan;
    LightController& _light;
    TouchSensor&     _touch;
    TaskHandle_t     _autoFanTask;
    TaskHandle_t     _autoLightTask;

    void run();
    void processCommand(char cmd);
};
