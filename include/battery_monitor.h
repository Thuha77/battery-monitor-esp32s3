#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>
#include <algorithm>

// =====================================================================
// CONFIG - edit these if your hardware changes
// =====================================================================
static const int ADC_PIN = 10;   // your ADC node pin

// Single combined calibration factor, derived from real measurements:
//   CAL_FACTOR = Vbat_actual (multimeter) / ADC_raw (chip reading)
//              = 7.56 / 1.954 = 3.869
// Re-derive this any time you change resistors, board, or wiring.
static const float CAL_FACTOR = 3.869f;

// Reference only (not used directly in the math, since CAL_FACTOR
// already absorbs these - kept here for documentation / future re-calibration)
static const float R1 = 2.93f;                    // kΩ, bottom resistor (ADC side)
static const float R2 = 9.74f;                    // kΩ, top resistor (battery side)
static const float INTERNAL_RESISTANCE_KOHM = 30.0f;

// Battery config (2S Li-ion)
static const uint8_t CELL_COUNT = 2;
static const float   CELL_MIN_V = 3.0f;   // per-cell -> 0%
static const float   CELL_MAX_V = 4.2f;   // per-cell -> 100%

static const int NUM_SAMPLES = 15;

// =====================================================================
// INTERNAL HELPERS
// =====================================================================
inline uint32_t _batteryReadFilteredMilliVolts() {
  uint32_t samples[NUM_SAMPLES];
  for (int i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = analogReadMilliVolts(ADC_PIN);
    delay(3);
  }
  std::sort(samples, samples + NUM_SAMPLES);
  return samples[NUM_SAMPLES / 2];   // median rejects noise spikes
}

// =====================================================================
// PUBLIC API
// =====================================================================

// Call once in setup()
inline void batteryMonitorInit() {
  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PIN, ADC_11db);
}

// Returns the battery pack voltage in volts (e.g. 7.56)
inline float getBatteryVoltage() {
  uint32_t mv_raw = _batteryReadFilteredMilliVolts();
  float adc_v_raw = mv_raw / 1000.0f;
  return adc_v_raw * CAL_FACTOR;
}

// Returns battery charge level as a percentage (0-100)
inline float getBatteryPercentage() {
  float vbat = getBatteryVoltage();
  float cellVoltage = vbat / CELL_COUNT;
  float pct = (cellVoltage - CELL_MIN_V) / (CELL_MAX_V - CELL_MIN_V) * 100.0f;
  if (pct > 100.0f) pct = 100.0f;
  if (pct < 0.0f)   pct = 0.0f;
  return pct;
}

#endif // BATTERY_MONITOR_H
