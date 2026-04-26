/**
 * @file    Config.h
 * @brief   Centralised hardware-pin map and tunable constants.
 *
 * Edit only this file when adapting the firmware to a different PCB layout
 * or threshold values.  No magic numbers should appear anywhere else in the
 * source tree.
 *
 * @author  Ahmed Abdelaziz (refactored)
 * @date    2026
 * @license MIT
 */

#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS core selection
// ─────────────────────────────────────────────────────────────────────────────
#if CONFIG_FREERTOS_UNICORE
static constexpr BaseType_t APP_CPU = 0;
#else
static constexpr BaseType_t APP_CPU = 1;
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Sensor GPIO pins
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint8_t PIN_DHT          = 33; ///< DHT11 data line
static constexpr uint8_t PIN_LIGHT_SENSOR = 26; ///< LDR analogue input
static constexpr uint8_t PIN_SMOKE_SENSOR = 25; ///< MQ-2 analogue input
static constexpr uint8_t PIN_TOUCH_SENSOR =  4; ///< ESP32 capacitive-touch (T0)
static constexpr uint8_t PIN_ECHO         =  2; ///< HC-SR04 echo
static constexpr uint8_t PIN_TRIGGER      = 15; ///< HC-SR04 trigger

// ─────────────────────────────────────────────────────────────────────────────
// Actuator GPIO pins
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint8_t PIN_FAN_RELAY    = 17; ///< Relay: fan
static constexpr uint8_t PIN_LIGHT_RELAY  = 16; ///< Relay: bulb

// ─────────────────────────────────────────────────────────────────────────────
// Alert GPIO pins
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint8_t PIN_SMOKE_BUZZER = 14; ///< Buzzer: smoke alert
static constexpr uint8_t PIN_TOUCH_BUZZER = 27; ///< Buzzer: intrusion alert
static constexpr uint8_t PIN_SMOKE_LED    =  5; ///< LED: smoke alert
static constexpr uint8_t PIN_TOUCH_LED    = 19; ///< LED: intrusion alert
static constexpr uint8_t PIN_ULTRASONIC_LED = 18; ///< LED: presence detected

// ─────────────────────────────────────────────────────────────────────────────
// OLED display parameters (I²C)
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint8_t  OLED_WIDTH   = 128; ///< Display width  (px)
static constexpr uint8_t  OLED_HEIGHT  =  64; ///< Display height (px)
static constexpr uint8_t  OLED_ADDR    = 0x3C; ///< I²C address
static constexpr int8_t   OLED_RESET   =   4; ///< Reset pin (-1 = share Arduino reset)

// ─────────────────────────────────────────────────────────────────────────────
// Bluetooth device name
// ─────────────────────────────────────────────────────────────────────────────
static constexpr const char* BT_DEVICE_NAME = "SmartHome-ESP32";

// ─────────────────────────────────────────────────────────────────────────────
// DHT sensor type
// ─────────────────────────────────────────────────────────────────────────────
#define DHT_TYPE DHT11

// ─────────────────────────────────────────────────────────────────────────────
// Threshold values
// ─────────────────────────────────────────────────────────────────────────────
static constexpr int   TEMP_FAN_ON_THRESHOLD    = 33;   ///< °C above which fan turns on
static constexpr int   LIGHT_BULB_ON_THRESHOLD  = 2200; ///< ADC value below which bulb turns on
static constexpr int   SMOKE_ALARM_THRESHOLD    = 3200; ///< ADC value above which smoke alarm fires
static constexpr int   TOUCH_ALARM_THRESHOLD    = 20;   ///< Capacitive reading below which touch is detected
static constexpr int   ULTRASONIC_RANGE_CM      = 20;   ///< cm: presence threshold

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS task periods (ms)
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint32_t PERIOD_TEMP_READ_MS      = 2000;
static constexpr uint32_t PERIOD_AUTO_FAN_MS       =  200;
static constexpr uint32_t PERIOD_LIGHT_READ_MS     = 2000;
static constexpr uint32_t PERIOD_AUTO_LIGHT_MS     =  200;
static constexpr uint32_t PERIOD_SMOKE_DETECT_MS   = 1000;
static constexpr uint32_t PERIOD_OLED_REFRESH_MS   =  200;
static constexpr uint32_t PERIOD_LOOP_DELAY_MS     =  500;

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS queue depths
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint8_t QUEUE_TEMP_DEPTH  = 10;
static constexpr uint8_t QUEUE_LIGHT_DEPTH = 10;

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS task stack sizes (words)
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint32_t STACK_TEMP_READ    = 2048;
static constexpr uint32_t STACK_AUTO_FAN     = 2048;
static constexpr uint32_t STACK_LIGHT_READ   = 1024;
static constexpr uint32_t STACK_AUTO_LIGHT   = 1024;
static constexpr uint32_t STACK_SMOKE        = 1024;
static constexpr uint32_t STACK_TOUCH        = 1024;
static constexpr uint32_t STACK_SWITCH_CTRL  = 4096;
static constexpr uint32_t STACK_OLED         = 8192;

// ─────────────────────────────────────────────────────────────────────────────
// Serial baud rate
// ─────────────────────────────────────────────────────────────────────────────
static constexpr uint32_t SERIAL_BAUD = 115200;
