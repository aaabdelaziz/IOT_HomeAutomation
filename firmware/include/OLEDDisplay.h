/**
 * @file    OLEDDisplay.h
 * @brief   SSD1306 128×64 OLED display driver for the smart-home dashboard.
 * @author  Mahmoud Ali
 * @date    2026
 * @license MIT
 */

#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Config.h"
#include "SystemState.h"

/**
 * @class OLEDDisplay
 * @brief Renders the smart-home dashboard on a 128×64 SSD1306 OLED.
 *
 * ### Screen layout
 * ```
 * ┌──────────────────────────────┐
 * │  Temp  27°C                  │  ← row 1 (large font)
 * │  [fan][bulb][smk][tch][usc]  │  ← row 2: 20×20 icons or "-"
 * └──────────────────────────────┘
 * ```
 *
 * Icons are drawn from PROGMEM bitmaps defined in Icons.h.
 * A dash character (`-`) is shown in place of an icon when the
 * corresponding subsystem is inactive.
 *
 * The splash screen is displayed once at startup for 4 seconds.
 */
class OLEDDisplay {
public:
    /**
     * @param displayQueue  Queue of temperature readings produced by
     *                      TemperatureSensor (the display blocks on this queue
     *                      so it refreshes in sync with new sensor data).
     */
    explicit OLEDDisplay(QueueHandle_t displayQueue);

    /// Initialise the I²C bus and SSD1306 driver — call once in setup().
    void begin();

    /// Show the splash logo + "Smart Home Automation" text.
    void showSplash();

    /// Render one full dashboard frame from the current SharedState snapshot.
    void refresh(int tempC);

    /// FreeRTOS task entry. Pass pointer to OLEDDisplay instance.
    static void taskEntry(void* param);

private:
    Adafruit_SSD1306 _display;
    QueueHandle_t    _displayQueue;

    /// Draw a status icon or a "-" placeholder at (x, y).
    void drawStatusIcon(const uint8_t* bitmap, int x, int y, bool active);

    void run();
};
