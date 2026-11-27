#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#define ARDUINO_ISR_ATTR
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define FALLING 3
using portMUX_TYPE = int;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL_ISR(x) ((void)(x))
#define portEXIT_CRITICAL_ISR(x) ((void)(x))
#define portENTER_CRITICAL(x) ((void)(x))
#define portEXIT_CRITICAL(x) ((void)(x))
inline uint32_t mockMs = 0;
inline unsigned long mockEcho = 0;
inline int pinValues[40] = {};
inline void (*interruptHandlers[40])() = {};
inline uint32_t millis() { return mockMs; }
inline void pinMode(int, int) {}
inline void digitalWrite(int pin, int value) { pinValues[pin] = value; }
inline void delayMicroseconds(unsigned long) {}
inline unsigned long pulseIn(int, int, unsigned long) { return mockEcho; }
inline int digitalPinToInterrupt(int pin) { return pin; }
inline void attachInterrupt(int pin, void (*handler)(), int) { interruptHandlers[pin] = handler; }
struct SerialMock {
  void begin(int) {}
  void println(const char*) {}
  template<class... Args> void printf(const char*, Args...) {}
};
inline SerialMock Serial;
