/**
 * @file    TouchSensor.cpp
 * @brief   ESP32 capacitive-touch intrusion sensor implementation.
 * @author  Mahmoud Ali
 * @date    2026
 * @license MIT
 */

#include "TouchSensor.h"
#include "BluetoothManager.h"
#include "SystemState.h"

void TouchSensor::begin() {
    pinMode(PIN_TOUCH_LED,    OUTPUT);
    pinMode(PIN_TOUCH_BUZZER, OUTPUT);
    digitalWrite(PIN_TOUCH_LED,    LOW);
    digitalWrite(PIN_TOUCH_BUZZER, LOW);
    _alarmActive = false;
}

void TouchSensor::clearAlarm() {
    _alarmActive = false;
    digitalWrite(PIN_TOUCH_LED,    LOW);
    digitalWrite(PIN_TOUCH_BUZZER, LOW);
    SharedState::update([](SystemState& s) { s.touchDetected = false; });
    BluetoothManager::instance().send("Touch inactive?");
    Serial.println(F("[TouchSensor] Alarm cleared by user command"));
}

bool TouchSensor::isAlarmActive() const {
    return _alarmActive;
}

/*static*/ void TouchSensor::taskEntry(void* param) {
    static_cast<TouchSensor*>(param)->run();
}

void TouchSensor::run() {
    while (true) {
        int touchValue = static_cast<int>(touchRead(PIN_TOUCH_SENSOR));

        if (touchValue < TOUCH_ALARM_THRESHOLD && !_alarmActive) {
            _alarmActive = true;
            digitalWrite(PIN_TOUCH_LED,    HIGH);
            digitalWrite(PIN_TOUCH_BUZZER, HIGH);
            SharedState::update([](SystemState& s) { s.touchDetected = true; });
            BluetoothManager::instance().send("Touch active?");
            Serial.println(F("[TouchSensor] ALARM — touch/intrusion detected!"));
        }
        // Note: alarm is only cleared externally via clearAlarm()
        // (requires an explicit 'T' command from the mobile app)

        // No explicit delay: tight loop so no touch event is missed.
        // TaskYIELD allows other tasks to run.
        taskYIELD();
    }
}
