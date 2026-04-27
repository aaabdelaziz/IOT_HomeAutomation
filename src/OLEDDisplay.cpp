/**
 * @file    OLEDDisplay.cpp
 * @brief   SSD1306 128×64 OLED display driver implementation.
 * @author  Mahmoud Ali
 * @date    2026
 * @license MIT
 */

#include "OLEDDisplay.h"
#include "Icons.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

OLEDDisplay::OLEDDisplay(QueueHandle_t displayQueue)
    : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET),
      _displayQueue(displayQueue) {}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

void OLEDDisplay::begin() {
    Wire.begin();
    if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("[OLEDDisplay] ERROR: SSD1306 allocation failed!"));
        // Halt — display is required for HMI
        for (;;) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }
    Serial.println(F("[OLEDDisplay] SSD1306 initialised OK"));
}

void OLEDDisplay::showSplash() {
    _display.clearDisplay();
    _display.drawBitmap(0, 0, Icons::splash, Icons::SPLASH_W, Icons::SPLASH_H, WHITE);
    _display.display();
    delay(4000);

    _display.clearDisplay();
    _display.setTextColor(WHITE);
    _display.setTextSize(2);
    _display.setCursor(6, 10);
    _display.print(F("Smart Home"));
    _display.setCursor(6, 40);
    _display.print(F("Automation"));
    _display.display();
    delay(4000);

    _display.clearDisplay();
    _display.display();
    Serial.println(F("[OLEDDisplay] Splash complete"));
}

void OLEDDisplay::refresh(int tempC) {
    SystemState state = SharedState::snapshot();

    _display.clearDisplay();

    // ── Temperature reading ───────────────────────────────────────────────
    _display.setTextColor(WHITE);
    _display.setTextSize(2);
    _display.setCursor(10, 10);
    _display.print(F("Temp "));
    _display.print(tempC);
    _display.print(static_cast<char>(247)); // degree symbol
    _display.print('C');

    // ── Status icons (row y = 36, x positions: 2, 28, 54, 80, 106) ───────
    drawStatusIcon(Icons::fan,         2,   36, state.fanActive);
    drawStatusIcon(Icons::light,       28,  36, state.lightActive);
    drawStatusIcon(Icons::smoke,       54,  36, state.smokeDetected);
    drawStatusIcon(Icons::touch,       80,  36, state.touchDetected);
    drawStatusIcon(Icons::ultrasonic, 106,  36, state.presenceDetected);

    _display.display();
}

// ─────────────────────────────────────────────────────────────────────────────
// Private helpers
// ─────────────────────────────────────────────────────────────────────────────

void OLEDDisplay::drawStatusIcon(const uint8_t* bitmap, int x, int y, bool active) {
    if (active) {
        _display.drawBitmap(x, y, bitmap, Icons::ICON_W, Icons::ICON_H, WHITE);
    } else {
        _display.setCursor(x + 4, y + 2);
        _display.setTextSize(1);
        _display.print('-');
        _display.setTextSize(2); // restore
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS task
// ─────────────────────────────────────────────────────────────────────────────

/*static*/ void OLEDDisplay::taskEntry(void* param) {
    static_cast<OLEDDisplay*>(param)->run();
}

void OLEDDisplay::run() {
    int tempValue = 0;
    while (true) {
        // Block until TemperatureSensor sends a fresh reading
        if (xQueueReceive(_displayQueue, &tempValue, portMAX_DELAY) == pdTRUE) {
            refresh(tempValue);
        }
        vTaskDelay(pdMS_TO_TICKS(PERIOD_OLED_REFRESH_MS));
    }
}
