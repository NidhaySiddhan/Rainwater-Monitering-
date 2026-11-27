# ESP32 Water Monitoring System

An Arduino C++ prototype for monitoring tank distance, cumulative outlet water use and tipping-bucket rainfall on a 1.3-inch SH1106 OLED.

## Features

- Counts outlet flow pulses and displays estimated water used in litres.
- Counts debounced rain-gauge switch closures on a separate input and displays rainfall in millimetres.
- Measures the air gap above the tank water with an ultrasonic sensor; displays full status or a sensor error on echo timeout.
- Prints raw pulse/tip counts and converted readings to Serial Monitor for calibration.
- Runs locally without Wi-Fi, accounts or API keys.

This version replaces the earlier `Leakage: DETECTED` indicator with water usage. Normal outlet use no longer sounds the buzzer; GPIO25 is held LOW. It does not detect leaks or control a pump.

**Calibration required:** the supplied 450 pulses/L and 0.5 mm/tip are starting values, not measurements of this prototype. An asterisk on the OLED marks uncalibrated readings. All totals reset when the ESP32 restarts or loses power. Rainfall depth is not tank volume, and litres used does not indicate litres remaining.

## Components and pins

| Component / signal | ESP32 GPIO |
| --- | --- |
| HC-SR04 trigger | 5 |
| HC-SR04 echo, through level conversion | 18 |
| YF-S201 outlet pulse signal, 3.3 V compatible | 27 |
| Tipping-bucket reed switch | 26 |
| SH1106 OLED SDA / SCL | 21 / 22 |
| Previous buzzer output (disabled) | 25 |

Use a classic ESP32 DevKit, an HC-SR04, a YF-S201 or equivalent calibrated pulse flow sensor, a tipping-bucket mechanism with a reed switch and magnet, and a 128×64 SH1106 I²C OLED at address `0x3C`. See [wiring and calibration](docs/SETUP.md) before connecting power.

## Upload with Arduino IDE

1. Install Arduino IDE and the **esp32 by Espressif Systems** board package using [Espressif's installation guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
2. Install **U8g2** by oliver in Library Manager. `Wire` comes with the board package.
3. Open `Water_Monitoring/Water_Monitoring.ino`; keep `config.h` in the same folder.
4. Select your exact board (typically **ESP32 Dev Module** for a classic DevKit) and USB port.
5. Review pin assignments and calibration constants in `config.h`, then Verify and Upload.
6. Open Serial Monitor at **115200 baud**, calibrate both sensors and repeat the checks in [TESTING.md](TESTING.md).

The display refreshes once per second. `Tank: OK` means only that the measured distance exceeds the full threshold; it is not a water-quality or low-level assessment. No litres-remaining estimate is implemented.

## Validation and limitations

Host simulation tests exercise the actual sketch with mocked hardware APIs: pulse totals, conversion, switch debounce, timer rollover, missing echoes and disabled buzzer behavior. They do **not** validate ESP32 compilation, electrical wiring, display fit at very large totals, sensor accuracy or physical performance. Hardware calibration and testing are still required for this updated version.

Future work: calibrated tank-volume estimation, persistent totals, rainfall intensity and optional remote monitoring. These are not included in this version.

## Project credits

Based on the Team SRN water-monitoring prototype: S Nidhay Siddhan, Rohit Kumar and Sanjay K.

## Technical references

- [Espressif Arduino GPIO and interrupts](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/gpio.html)
- [Seeed Studio flow sensor explanation and calibration example](https://wiki.seeedstudio.com/Water-Flow-Sensor/)
- [U8g2 library](https://github.com/olikraus/u8g2)
