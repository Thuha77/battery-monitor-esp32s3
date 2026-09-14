# ESP32-S3 2S Li-ion Battery Voltage & Percentage Monitor

A lightweight PlatformIO/Arduino library for the ESP32-S3 that reads a 2S
Li-ion battery pack's voltage through a resistor voltage divider and
reports both the pack voltage and an estimated charge percentage.

## Features

- Reads battery voltage through a resistor divider safely within the
  ESP32-S3 ADC's input range
- Median-filtered sampling to reject noise spikes / outlier ADC readings
- Single empirical calibration factor that corrects for ESP32 ADC gain
  error, resistor tolerance, and any parasitic/internal circuit
  resistance — all in one measured constant
- Simple two-function API: get raw voltage or get percentage directly
- Configured for a 2S pack (3.0V/cell = 0%, 4.2V/cell = 100%), easily
  adjustable for other pack configurations

## Hardware

- ESP32-S3 dev board
- 2S Li-ion battery pack (nominal 7.4V, full charge ~8.4V)
- Voltage divider: R1 = 2.93kΩ (bottom, ADC side), R2 = 9.74kΩ (top,
  battery side)

```
BAT+ ---[ R2 ]---+---[ R1 ]--- GND
                  |
               ADC_PIN
```

ADC pin used: **GPI10** (configurable in `battery_monitor.h`). Any
ADC1-capable pin works — see the "ADC pin notes" section below.

## Project structure

```
.
├── include/
│   └── battery_monitor.h   # core library - read this for the API
├── src/
│   └── main.cpp             # example usage
└── platformio.ini
```

## Usage

```cpp
#include "battery_monitor.h"

void setup() {
  batteryMonitorInit();          // call once
}

void loop() {
  float pct = getBatteryPercentage();  // 0-100
  float v   = getBatteryVoltage();     // pack voltage in volts
}
```

## Calibration

The library uses a single empirical calibration constant (`CAL_FACTOR`)
rather than pure resistor-value math, since real-world resistor
tolerance, ESP32 ADC non-linearity, and any parasitic board resistance
all introduce small errors that are easier to correct for all at once
than to model individually.

To (re-)calibrate for your own hardware:

1. Flash the firmware and let the `ADC_raw` value in the serial output
   settle.
2. Measure the battery pack directly with a multimeter.
3. Compute:
   ```
   CAL_FACTOR = Vbat_multimeter / ADC_raw
   ```
4. Update `CAL_FACTOR` in `battery_monitor.h`.

Re-run this whenever you change the resistor values, the board, or the
wiring.

## ADC pin notes (ESP32-S3)

- Prefer **ADC1 pins (GPIO1–GPIO10)** — ADC2 shares hardware with the
  WiFi radio and can give unreliable readings when WiFi is active.
- Avoid GPIO0 (boot strap / BOOT button) and GPIO3 (strapping pin).
- Avoid GPIO19/GPIO20 on boards using native USB (D-/D+ lines).

## License

MIT