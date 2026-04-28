/**
 * @file    SmokeSensor.cpp
 * @brief   MQ-2 smoke/gas sensor driver implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "SmokeSensor.h"
#include "BluetoothManager.h"
#include "SystemState.h"

void SmokeSensor::begin() {
    pinMode(PIN_SMOKE_LED,    OUTPUT);
    pinMode(PIN_SMOKE_BUZZER, OUTPUT);
    digitalWrite(PIN_SMOKE_LED,    LOW);
    digitalWrite(PIN_SMOKE_BUZZER, LOW);
    _alarmActive = false;
}

void SmokeSensor::update() {
    int smokeValue = analogRead(PIN_SMOKE_SENSOR);
    Serial.printf("[SmokeSensor] Raw ADC: %d\n", smokeValue);

    SharedState::update([smokeValue](SystemState& s) { s.smokeLevel = smokeValue; });

    if (smokeValue >= SMOKE_ALARM_THRESHOLD && !_alarmActive) {
        _alarmActive = true;
        digitalWrite(PIN_SMOKE_LED,    HIGH);
        digitalWrite(PIN_SMOKE_BUZZER, HIGH);
        SharedState::update([](SystemState& s) { s.smokeDetected = true; });
        BluetoothManager::instance().send("Smoke active?");
        Serial.println(F("[SmokeSensor] ALARM — smoke/gas detected!"));
    } else if (smokeValue < SMOKE_ALARM_THRESHOLD && _alarmActive) {
        _alarmActive = false;
        digitalWrite(PIN_SMOKE_LED,    LOW);
        digitalWrite(PIN_SMOKE_BUZZER, LOW);
        SharedState::update([](SystemState& s) { s.smokeDetected = false; });
        BluetoothManager::instance().send("Smoke inactive?");
        Serial.println(F("[SmokeSensor] Alarm cleared"));
    }
}

bool SmokeSensor::isAlarmActive() const {
    return _alarmActive;
}

/*static*/ void SmokeSensor::taskEntry(void* param) {
    static_cast<SmokeSensor*>(param)->run();
}

void SmokeSensor::run() {
    while (true) {
        update();
        vTaskDelay(pdMS_TO_TICKS(PERIOD_SMOKE_DETECT_MS));
    }
}
