#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t pendingFlowPulses = 0;
volatile uint32_t pendingRainTips = 0;
volatile uint32_t lastRainTipMs = 0;
volatile bool rainTipSeen = false;
uint64_t totalFlowPulses = 0;
uint64_t totalRainTips = 0;
uint32_t lastUpdateMs = 0;
float distanceCm = -1;

void ARDUINO_ISR_ATTR flowPulse() {
  portENTER_CRITICAL_ISR(&pulseMux);
  ++pendingFlowPulses;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

void ARDUINO_ISR_ATTR rainTip() {
  const uint32_t now = millis();
  portENTER_CRITICAL_ISR(&pulseMux);
  // Unsigned subtraction also works when millis() wraps around.
  if (!rainTipSeen || uint32_t(now - lastRainTipMs) >= RAIN_DEBOUNCE_MS) {
    ++pendingRainTips;
    lastRainTipMs = now;
    rainTipSeen = true;
  }
  portEXIT_CRITICAL_ISR(&pulseMux);
}

void collectPulses() {
  uint32_t flow, rain;
  portENTER_CRITICAL(&pulseMux);
  flow = pendingFlowPulses;
  rain = pendingRainTips;
  pendingFlowPulses = 0;
  pendingRainTips = 0;
  portEXIT_CRITICAL(&pulseMux);
  totalFlowPulses += flow;
  totalRainTips += rain;
}

double litresUsed() {
  return double(totalFlowPulses) / FLOW_PULSES_PER_LITRE;
}

double rainfallMm() {
  return double(totalRainTips) * RAIN_MM_PER_TIP;
}

float getDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  const unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  return duration == 0 ? -1.0f : duration * 0.0343f / 2.0f;
}

void showDisplay() {
  char line[48];
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tf);
  display.drawStr(0, 10, "WATER MONITOR");
  snprintf(line, sizeof(line), "Used: %.2f L%s", litresUsed(), FLOW_CALIBRATED ? "" : "*");
  display.drawStr(0, 22, line);
  snprintf(line, sizeof(line), "Rain: %.2f mm%s", rainfallMm(), RAIN_CALIBRATED ? "" : "*");
  display.drawStr(0, 34, line);
  if (distanceCm <= 0) {
    display.drawStr(0, 46, "Tank: SENSOR ERROR");
  } else {
    snprintf(line, sizeof(line), "Tank: %s %.1fcm", distanceCm <= FULL_DISTANCE_CM ? "FULL" : "OK", distanceCm);
    display.drawStr(0, 46, line);
  }
  display.drawStr(0, 61, (FLOW_CALIBRATED && RAIN_CALIBRATED) ? "Totals since restart" : "* Calibrate sensors");
  display.sendBuffer();
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
  pinMode(FLOW_PIN, INPUT_PULLUP);
  pinMode(RAIN_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  // Normal water use is not a leak. Leave the previous buzzer disabled.
  digitalWrite(BUZZER_PIN, LOW);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowPulse, FALLING);
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTip, FALLING);
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  display.setI2CAddress(OLED_ADDRESS * 2);
  display.begin();
  Serial.println("Water monitor: session totals reset on restart.");
  Serial.println("Calibrate both sensors before relying on L/mm values.");
  lastUpdateMs = millis();
}

void loop() {
  const uint32_t now = millis();
  if (uint32_t(now - lastUpdateMs) < UPDATE_INTERVAL_MS) return;
  lastUpdateMs = now;
  distanceCm = getDistanceCm();
  collectPulses();
  showDisplay();
  Serial.printf("flow_pulses=%llu, used_L=%.3f, rain_tips=%llu, rain_mm=%.3f, distance_cm=%.1f\n",
                (unsigned long long)totalFlowPulses, litresUsed(),
                (unsigned long long)totalRainTips, rainfallMm(), distanceCm);
}
