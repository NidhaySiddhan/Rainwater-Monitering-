# Validation

## Host simulation

From the repository root, with a C++17 compiler:

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Werror -Itests/stubs tests/test_monitor.cpp -o build/test_monitor
./build/test_monitor
```

The test includes the actual sketch and substitutes Arduino, display and interrupt APIs. It checks cumulative flow conversion, rain conversion, bounce rejection, first-tip handling, timer wraparound, counter draining without duplicate counts, ultrasonic timeout, display status and the inactive buzzer. It does not simulate concurrent interrupt execution or ESP32 hardware.

## Physical validation checklist — not yet performed for this update

- Verify and upload using the exact ESP32 board and U8g2 library.
- Confirm the OLED fits and shows initial zero totals with calibration markers.
- With no flow or rain, verify pulse and tip totals remain stationary.
- Collect known water volumes and calibrate outlet pulses/L; repeat at several flow rates.
- Tip the rain bucket manually and check accepted counts, then calibrate using measured water volumes.
- Disconnect the ultrasonic Echo input: confirm SENSOR ERROR rather than FULL.
- Check full threshold and normal readings against known distances.
- Confirm normal outlet use does not activate the buzzer.
- Run flow and rain inputs simultaneously while the OLED updates; compare counts with known references.
- Restart the controller and verify session totals reset to zero.
- Operate the model for 20–30 minutes and record observed errors, resets and any missed pulses.

Do not claim accuracy, water savings or hardware validation until measured results are recorded.
