#pragma once
#include <string>
#include <vector>
#define U8G2_R0 0
#define U8X8_PIN_NONE 0
inline const char *u8g2_font_6x10_tf = "mock-font";
struct U8G2_SH1106_128X64_NONAME_F_HW_I2C {
  std::vector<std::string> lines;
  U8G2_SH1106_128X64_NONAME_F_HW_I2C(int, int) {}
  void clearBuffer() { lines.clear(); }
  void setFont(const char*) {}
  void drawStr(int, int, const char* text) { lines.emplace_back(text); }
  void sendBuffer() {}
  void setI2CAddress(int) {}
  void begin() {}
};
