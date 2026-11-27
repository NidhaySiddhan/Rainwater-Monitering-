#include <cassert>
#include <cmath>
#include <iostream>
#include "../Water_Monitoring/Water_Monitoring.ino"

int main() {
  setup();
  assert(interruptHandlers[FLOW_PIN] == flowPulse);
  assert(interruptHandlers[RAIN_PIN] == rainTip);
  assert(pinValues[BUZZER_PIN] == LOW);
  assert(litresUsed() == 0 && rainfallMm() == 0);

  for (int i = 0; i < 900; ++i) flowPulse();
  // First closure at boot accepted; bounce at 1ms and 49ms rejected.
  rainTip(); mockMs = 1; rainTip(); mockMs = 49; rainTip();
  mockMs = 50; rainTip();
  collectPulses();
  assert(totalFlowPulses == 900 && totalRainTips == 2);
  assert(std::abs(litresUsed() - 2.0) < 1e-9);
  assert(std::abs(rainfallMm() - 1.0) < 1e-9);
  collectPulses();
  assert(totalFlowPulses == 900 && totalRainTips == 2); // no double count

  // Rain debounce across the uint32_t timer rollover.
  lastRainTipMs = UINT32_MAX - 20;
  mockMs = 10; rainTip(); // 31ms: ignored
  mockMs = 30; rainTip(); // 51ms: accepted
  collectPulses();
  assert(totalRainTips == 3);

  mockEcho = 0;
  assert(getDistanceCm() == -1);
  distanceCm = getDistanceCm(); showDisplay();
  assert(display.lines[3] == "Tank: SENSOR ERROR");
  assert(display.lines[1] == "Used: 2.00 L*");
  assert(display.lines[2] == "Rain: 1.50 mm*");
  mockEcho = 290; distanceCm = getDistanceCm(); showDisplay();
  assert(display.lines[3].find("Tank: FULL") == 0);
  mockEcho = 1000; distanceCm = getDistanceCm(); showDisplay();
  assert(display.lines[3].find("Tank: OK") == 0);

  // Loop scheduling survives wraparound and drains new pulses once.
  lastUpdateMs = UINT32_MAX - 500;
  mockMs = 499; flowPulse(); loop();
  assert(totalFlowPulses == 901);
  assert(pinValues[BUZZER_PIN] == LOW);
  flowPulse(); loop();
  assert(totalFlowPulses == 901);
  mockMs += 1000; loop();
  assert(totalFlowPulses == 902);
  std::cout << "PASS: flow volume, rain conversion, debounce, rollover, echo errors, display and buzzer\n";
}
