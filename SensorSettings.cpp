// Copyright 2020 Brett M. Leedy
#include "SensorSettings.hh"
#ifdef IN_TESTING
#include "test/Stub_EEPROM.hh"
#else
#include "EEPROM.h"
#endif

  SensorSettings::SensorSettings() {
    // When instantiated, try to load myself from EEPROM.
    load_settings_from_eeprom(0, &data);
    if (this->check()) {
      Serial.println(F("Valid config on eeprom."));
      this->dump();
    } else {
      Serial.println(F("Checksum failed! Writing default config."));
      this->data.sampling_period_ms = DEFAULT_LOOP_PERIOD_MILLIS;
      this->data.log_every_n_loops = DEFAULT_LOG_EVERY_N_LOOPS;
      this->data.log_raw = false;
      this->data.file_number = 0;
      this->data.backlight = true;
      this->data.logging = true;
      this->data.alt_sensor_config = false;
      this->data.display_raw = false;
      commit();  // write to EEPROM with valid checksum
    }
  }

  // light wrapper for better testing - DO NOT ADD LOGIC HERE
  void SensorSettings::load_settings_from_eeprom(int idx,
                                                SettingsType * myconfig) {
    EEPROM.get(idx, *myconfig);
  }

  uint32_t SensorSettings::calc_checksum() {
    uint32_t rv = 198;
    rv += this->data.sampling_period_ms;
    rv += this->data.log_every_n_loops;
    rv += this->data.log_raw;
    rv += this->data.file_number;
    rv += this->data.backlight;
    rv += this->data.alt_sensor_config;
    for (int i = 0; i < SENSORSTATE_MAX_NUM_SENSORS; i++) {
      rv += this->data.sensor_thresholds[i];
    }
    rv += this->data.logging;
    rv += this->data.display_raw;
    return rv;
  }

  void SensorSettings::dump() {
    Serial.println(F("Configuration:"));
    Serial.print(F("  sampling_period_ms: "));
    Serial.println(this->data.sampling_period_ms);
    Serial.print(F("  log_every_n_loops: "));
    Serial.println(this->data.log_every_n_loops);
    Serial.print(F("  log_raw (boolean): "));
    Serial.println(this->data.log_raw);
    Serial.print(F("  file_number: "));
    Serial.println(this->data.file_number);
    Serial.print(F("  backlight: "));
    Serial.println(this->data.backlight);
    Serial.print(F("  logging: "));
    Serial.println(this->data.logging);
    Serial.print(F("  checksum: "));
    Serial.println(this->data.checksum);
    Serial.print(F("  alt_sensor_config: "));
    Serial.println(this->data.alt_sensor_config);
    Serial.print(F("  calculated checksum: "));
    Serial.println(this->calc_checksum());
  }

  void SensorSettings::store_checksum() {
    this->data.checksum = calc_checksum();
  }

  bool SensorSettings::check() {
    return (this->data.checksum == calc_checksum());
  }

  void SensorSettings::commit() {
    this->store_checksum();
    EEPROM.put(0, this->data);
  }
