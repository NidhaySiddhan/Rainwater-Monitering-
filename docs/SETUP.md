# Wiring and calibration

## Electrical connections

Disconnect power before changing wiring. Keep the controller and connections dry.

- Connect all module grounds to ESP32 GND. Use the supply voltage specified for each purchased module, not its signal pin voltage.
- HC-SR04 TRIG goes to GPIO5. A standard HC-SR04 Echo is a 5 V signal: use a suitable level shifter or resistor divider before GPIO18. Do not connect a 5 V output directly to an ESP32 input. For a nominal 5 V Echo, 10 kΩ from Echo to GPIO18 and 15 kΩ from GPIO18 to GND produces about 3 V; verify with your module.
- Flow signal goes to GPIO27 through any required level conversion. Check whether your sensor has an open-collector output or a driven output. Use a pull-up to **3.3 V** for an appropriate open-collector output; never pull an ESP32 input to 5 V. Follow the flow-direction arrow on the sensor.
- Connect the rain gauge's dry-contact reed switch between GPIO26 and GND. The sketch enables the internal pull-up and counts falling edges. One counted closure must correspond to the calibrated amount of rain. Do not apply external voltage to this switch input.
- OLED SDA goes to GPIO21 and SCL to GPIO22. Verify supply and pull-up voltages are compatible with ESP32 3.3 V logic. The sketch uses SH1106, not SSD1306.
- The previous buzzer output at GPIO25 remains LOW. It can be left disconnected. No water-use alarm is enabled.

GPIO5 is a strapping pin on the classic ESP32; retain the prototype assignment only if attached hardware permits normal boot. This pin map is not interchangeable with ESP32-C3/S3 boards without checking their pinouts.

## Flow sensor: litres used

1. Start a fresh session and note the raw `flow_pulses` value.
2. Pass a measured volume through the sensor into a measuring jug at the intended operating flow.
3. Note the increase in pulses. Calculate `FLOW_PULSES_PER_LITRE = pulse increase / collected litres`.
4. Repeat several times and use total pulses divided by total measured litres. Validate with a separate measured run, including the lowest intended flow.
5. Update the constant in `config.h`. Set `FLOW_CALIBRATED = true` only after validation, then upload again.

For example, 920 pulses for 2 L gives 460 pulses/L. The shipped value of 450 is a nominal starting point for a YF-S201; low gravity-fed flow may fail to turn the rotor and cannot be fixed by scaling alone. Pulse noise also looks like water use, so check for stationary counts with no flow.

`Used litres = total flow pulses / calibrated pulses per litre`.

## Tipping-bucket gauge: rainfall depth

1. Level the gauge and position the magnet so switch closures reliably match the mechanism's movement.
2. Tip it manually at least 20 times. Check `rain_tips` against the physical count, including both sides of a two-bucket rocker. A mechanism that closes only once per full cycle needs calibration per **counted closure**, not per physical half-cycle.
3. Measure the funnel opening area A in cm². For a circular opening, A = π × radius².
4. Slowly pour a measured volume V in mL, count N accepted closures, and start/end at equivalent bucket positions to reduce partial-fill error. Repeat multiple runs.
5. Calculate `RAIN_MM_PER_TIP = 10 × V / (A × N)` and validate with a separate run.
6. Update `RAIN_MM_PER_TIP` and set `RAIN_CALIBRATED = true` only after validation.

The supplied 0.5 mm/tip is an example. The 50 ms debounce window rejects rapid repeat edges; tune it using the real switch. It cannot prevent false counts from long electrical disturbances. If genuine closures occur within that window they are discarded.

Rainfall in millimetres is a depth over the funnel opening. It is not litres harvested by a roof or litres in the tank.

## Tank sensor

Mount the ultrasonic sensor above the water, away from splashes, pointing perpendicular to the surface. Compare readings with a ruler and set `FULL_DISTANCE_CM` for your tank while respecting the sensor's valid range. Timeout displays `SENSOR ERROR`; no automatic valve or overflow protection is provided. The sketch does not compute water remaining in litres.

## Session behavior

Totals begin at zero after every boot. They are neither daily totals nor persistent readings. Save Serial Monitor output if a measurement record is needed. Reflashing calibration constants starts a new session.
