/**
 * @file    UltrasonicSensor.cpp
 * @brief   HC-SR04 ultrasonic sensor implementation.
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#include "UltrasonicSensor.h"
#include "BluetoothManager.h"
#include "SystemState.h"

void UltrasonicSensor::begin() {
    pinMode(PIN_TRIGGER,       OUTPUT);
    pinMode(PIN_ECHO,          INPUT);
    pinMode(PIN_ULTRASONIC_LED, OUTPUT);
    digitalWrite(PIN_ULTRASONIC_LED, LOW);
    digitalWrite(PIN_TRIGGER,        LOW);
}

int UltrasonicSensor::getDistanceCm() const {
    return _distanceCm;
}

bool UltrasonicSensor::isPresenceDetected() const {
    return _presenceDetected;
}

/*static*/ void UltrasonicSensor::taskEntry(void* pvParameters) {
    UltrasonicSensor* sensor = static_cast<UltrasonicSensor*>(pvParameters);
    while (true) {
        sensor->measure();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void UltrasonicSensor::measure() {
    // Generate 10 µs trigger pulse
    digitalWrite(PIN_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(PIN_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIGGER, LOW);

    long duration = pulseIn(PIN_ECHO, HIGH);
    _distanceCm   = static_cast<int>((duration / 2) * 0.0343f);

    Serial.printf("[UltrasonicSensor] Distance: %d cm\n", _distanceCm);

    SharedState::update([this](SystemState& s) { s.distanceCm = _distanceCm; });

    if (_distanceCm <= ULTRASONIC_RANGE_CM) {
        if (!_presenceDetected) {
            _presenceDetected = true;
            digitalWrite(PIN_ULTRASONIC_LED, HIGH);
            SharedState::update([](SystemState& s) { s.presenceDetected = true; });
            BluetoothManager::instance().send("Ultrasonic active?");
            Serial.println(F("[UltrasonicSensor] Presence detected"));
        }
    } else {
        if (_presenceDetected) {
            _presenceDetected = false;
            digitalWrite(PIN_ULTRASONIC_LED, LOW);
            SharedState::update([](SystemState& s) { s.presenceDetected = false; });
            BluetoothManager::instance().send("Ultrasonic inactive?");
        }
    }
}
