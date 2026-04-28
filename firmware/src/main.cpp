/**
 * @file    main.cpp
 * @brief   Main entry point for the Smart Home Automation RTOS project.
 *
 * Bootstraps the system:
 *  - Initialises hardware peripherals and shared state.
 *  - Creates FreeRTOS queues.
 *  - Spawns all independent sensor and actuator tasks.
 *
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include <Arduino.h>
#include <Wire.h>
#include <soc/rtc_cntl_reg.h>

#include "Config.h"
#include "SystemState.h"
#include "BluetoothManager.h"
#include "TemperatureSensor.h"
#include "FanController.h"
#include "LightSensor.h"
#include "LightController.h"
#include "SmokeSensor.h"
#include "TouchSensor.h"
#include "UltrasonicSensor.h"
#include "OLEDDisplay.h"
#include "SwitchController.h"

// ─────────────────────────────────────────────────────────────────────────────
// Global Queues & Task Handles
// ─────────────────────────────────────────────────────────────────────────────

QueueHandle_t tempQueue    = nullptr;
QueueHandle_t lightQueue   = nullptr;
QueueHandle_t displayQueue = nullptr;

TaskHandle_t autoFanTask   = nullptr;
TaskHandle_t autoLightTask = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// Global Objects (Dynamically allocated in setup to ensure proper init order)
// ─────────────────────────────────────────────────────────────────────────────

TemperatureSensor* tempSensor  = nullptr;
FanController*     fanCtrl     = nullptr;
LightSensor*       lightSensor = nullptr;
LightController*   lightCtrl   = nullptr;
SmokeSensor*       smokeSensor = nullptr;
TouchSensor*       touchSensor = nullptr;
UltrasonicSensor*  ultraSensor = nullptr;
OLEDDisplay*       display     = nullptr;
SwitchController*  switchCtrl  = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// Setup
// ─────────────────────────────────────────────────────────────────────────────

void setup() {
    // Disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    Serial.begin(SERIAL_BAUD);
    Serial.println(F("\n\n--- Smart Home Automation Starting ---"));

    // 1. Core Initialisation
    SharedState::init();
    BluetoothManager::instance().begin();

    // 2. Create FreeRTOS Queues
    tempQueue    = xQueueCreate(QUEUE_TEMP_DEPTH,  sizeof(int));
    displayQueue = xQueueCreate(QUEUE_TEMP_DEPTH,  sizeof(int));
    lightQueue   = xQueueCreate(QUEUE_LIGHT_DEPTH, sizeof(int));

    // 3. Instantiate Subsystems
    tempSensor  = new TemperatureSensor(tempQueue, displayQueue);
    fanCtrl     = new FanController(tempQueue);
    lightSensor = new LightSensor(lightQueue);
    lightCtrl   = new LightController(lightQueue);
    smokeSensor = new SmokeSensor();
    touchSensor = new TouchSensor();
    ultraSensor = new UltrasonicSensor();
    display     = new OLEDDisplay(displayQueue);

    // 4. Hardware Begin
    tempSensor->begin();
    fanCtrl->begin();
    lightSensor->begin();
    lightCtrl->begin();
    smokeSensor->begin();
    touchSensor->begin();
    ultraSensor->begin();
    display->begin();

    // Show splash screen synchronously before starting tasks
    display->showSplash();

    // 5. Create FreeRTOS Tasks
    // Note: Ultrasonic uses a hardware timer (Ticker), no task needed.

    xTaskCreatePinnedToCore(
        TemperatureSensor::taskEntry, "TempRead", STACK_TEMP_READ,
        tempSensor, 1, nullptr, APP_CPU);

    xTaskCreatePinnedToCore(
        FanController::autoTaskEntry, "AutoFan", STACK_AUTO_FAN,
        fanCtrl, 1, &autoFanTask, APP_CPU);

    xTaskCreatePinnedToCore(
        UltrasonicSensor::taskEntry, "Ultrasonic", STACK_ULTRASONIC,
        ultraSensor, 1, nullptr, APP_CPU);

    xTaskCreatePinnedToCore(
        LightSensor::taskEntry, "LightRead", STACK_LIGHT_READ,
        lightSensor, 1, nullptr, APP_CPU);

    xTaskCreatePinnedToCore(
        LightController::autoTaskEntry, "AutoLight", STACK_AUTO_LIGHT,
        lightCtrl, 1, &autoLightTask, APP_CPU);

    xTaskCreatePinnedToCore(
        SmokeSensor::taskEntry, "SmokeDet", STACK_SMOKE,
        smokeSensor, 1, nullptr, APP_CPU);

    xTaskCreatePinnedToCore(
        TouchSensor::taskEntry, "TouchDet", STACK_TOUCH,
        touchSensor, 1, nullptr, APP_CPU);

    xTaskCreatePinnedToCore(
        OLEDDisplay::taskEntry, "OLED", STACK_OLED,
        display, 1, nullptr, APP_CPU);

    // Finally, instantiate and start the SwitchController with the auto-task handles
    switchCtrl = new SwitchController(*fanCtrl, *lightCtrl, *touchSensor,
                                      autoFanTask, autoLightTask);

    xTaskCreatePinnedToCore(
        SwitchController::taskEntry, "SwitchCtrl", STACK_SWITCH_CTRL,
        switchCtrl, 1, nullptr, APP_CPU);

    // 6. Default Mode: Suspend auto tasks at start (Manual Mode active)
    vTaskSuspend(autoFanTask);
    vTaskSuspend(autoLightTask);

    Serial.println(F("System bootstrap complete. FreeRTOS scheduler taking over."));
}

// ─────────────────────────────────────────────────────────────────────────────
// Loop
// ─────────────────────────────────────────────────────────────────────────────

void loop() {
    // All logic is RTOS-driven. Just yield to IDLE task.
    vTaskDelay(pdMS_TO_TICKS(PERIOD_LOOP_DELAY_MS));
}
