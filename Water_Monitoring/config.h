#pragma once
#include <stdint.h>

// Classic ESP32 DevKit pin assignments. Check your exact board before wiring.
constexpr uint8_t TRIG_PIN = 5;
constexpr uint8_t ECHO_PIN = 18;
constexpr uint8_t FLOW_PIN = 27;
constexpr uint8_t RAIN_PIN = 26;  // New reed-switch input; do not share GPIO27.
constexpr uint8_t BUZZER_PIN = 25;
constexpr uint8_t OLED_SDA_PIN = 21;
constexpr uint8_t OLED_SCL_PIN = 22;
constexpr uint8_t OLED_ADDRESS = 0x3C;
constexpr float FULL_DISTANCE_CM = 6.0f;
constexpr uint32_t UPDATE_INTERVAL_MS = 1000;
constexpr uint32_t RAIN_DEBOUNCE_MS = 50;

// Starting values, NOT measured calibration for your hardware.
// Flow: total pulses / measured litres collected.
constexpr double FLOW_PULSES_PER_LITRE = 450.0;
// Rain: 10 * volume per counted switch closure (mL) / funnel area (cm^2).
constexpr double RAIN_MM_PER_TIP = 0.5;
constexpr bool FLOW_CALIBRATED = false;
constexpr bool RAIN_CALIBRATED = false;

static_assert(FLOW_PULSES_PER_LITRE > 0, "Flow calibration must be positive");
static_assert(RAIN_MM_PER_TIP > 0, "Rain calibration must be positive");
static_assert(RAIN_PIN != FLOW_PIN, "Rain and flow need separate GPIO inputs");
