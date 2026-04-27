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

#include "LightSensor.h"
#include "LightController.h"

// ─────────────────────────────────────────────────────────────────────────────
// Global Queues & Task Handles
// ─────────────────────────────────────────────────────────────────────────────

QueueHandle_t lightQueue   = nullptr;
TaskHandle_t autoLightTask = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// Global Objects (Dynamically allocated in setup to ensure proper init order)
// ─────────────────────────────────────────────────────────────────────────────

LightSensor*       lightSensor = nullptr;
LightController*   lightCtrl   = nullptr;

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

    // 2. Create FreeRTOS Queues
    lightQueue   = xQueueCreate(QUEUE_LIGHT_DEPTH, sizeof(int));

    // 3. Instantiate Subsystems
    lightSensor = new LightSensor(lightQueue);
    lightCtrl   = new LightController(lightQueue);

    // 4. Hardware Begin
    lightSensor->begin();
    lightCtrl->begin();


    // 5. Create FreeRTOS Tasks
    // Note: Ultrasonic uses a hardware timer (Ticker), no task needed.

    xTaskCreatePinnedToCore(
        LightSensor::taskEntry, "LightRead", STACK_LIGHT_READ,
        lightSensor, 1, nullptr, APP_CPU);

    xTaskCreatePinnedToCore(
        LightController::autoTaskEntry, "AutoLight", STACK_AUTO_LIGHT,
        lightCtrl, 1, &autoLightTask, APP_CPU);

    Serial.println(F("System bootstrap complete. FreeRTOS scheduler taking over."));
}

// ─────────────────────────────────────────────────────────────────────────────
// Loop
// ─────────────────────────────────────────────────────────────────────────────

void loop() {
    // All logic is RTOS-driven. Just yield to IDLE task.
    vTaskDelay(pdMS_TO_TICKS(PERIOD_LOOP_DELAY_MS));
}
