// Copyright 2020 Brett M. Leedy
#include "DiyAirSensor.hh"

/*
  At init, instantiate all other components.  Here we:
    * Start the serial port
    * Configure the Digital I/O pins for buttons
    * Instantiate the LogFile class to which we write logs
    * Add and configre all of the sensors
    * Update my configs with my sensor info
    * Start up the LCD display

  At construction, class variable SensorState will also read settings from EEPROM.
*/
// Initializes serial
// Inits pins
// Instantiates the sensors, config, and LCD
// Updates the system state with sensor info
// Calls serial a few times to update us on what's happening
DiyAirSensor::DiyAirSensor(bool do_init) {
  if (do_init)
    this->initializeSensor();
}
void DiyAirSensor::initializeSensor() {
  if (initialized) {
    Serial.println(F("WARNING: Initialization will only run once."));
    return;
  }

  this->loop_number = 0;        // state for loop control
  this->loop_start_millis = 0;
  this->day_start_millis = this->getMillis();

  Serial.println(F("Init Button Pins..."));
  this->initPins();

  // Serial.println(F("Init Logfile..."));
  logfile = new LogFileSender();
  Serial.println(F("Init Sensors..."));
  sensors = new SensorArray();
  this->addSensors(sensors);

  Serial.println(F("Init Configs..."));
  this->updateConfigsFromSensor();

  Serial.println(F("Init LCD..."));
  sensor_display = new AirSensorDisplay();

  Serial.println(F("Init done."));
  this->initialized = true;
}

void DiyAirSensor::updateConfigsFromSensor() {
  sensors->write_sensor_configs(&sensor_state);
}

// Adds sensors to the sensor array, depending on config
void DiyAirSensor::addSensors(SensorArray * array) {
  if (!sensor_state.device.settings.data.alt_sensor_config) {
    array->add_sensor(new MQSensor(" LPG",  // MQ5 - LPG, City Gas Leak
      SENSOR_ACCUM_RATE, A1, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("  CO",  // MQ7 - Carbon Monoxide
      SENSOR_ACCUM_RATE, A6, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("Ozon",  // MQ131 - Ozone
      SENSOR_ACCUM_RATE, A7, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor(" Gas",  // MQ9 Gas leaks
      SENSOR_ACCUM_RATE, A3, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor(" Haz",  // MQ135Poison Gasses (organic)
      SENSOR_ACCUM_RATE, A2, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new GP2YSensor("Dust",  // PM2.5 sensor
      SENSOR_ACCUM_RATE, A0, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN, 4));
  } else {
    array->add_sensor(new MQSensor("MQ2 ",  // MQ2 - smoke
      SENSOR_ACCUM_RATE, A0, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("MQ7 ",  // MQ7 - Carbon Monoxide
      SENSOR_ACCUM_RATE, A1, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("MQ4 ",  // MQ4 - Methane (CNG)
      SENSOR_ACCUM_RATE, A2, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("MQ6 ",  // MQ-6 - LPG, iso-butane, propane
      SENSOR_ACCUM_RATE, A3, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("MQ8 ",  // MQ-8 - Hydrogen
      SENSOR_ACCUM_RATE, A6, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
    array->add_sensor(new MQSensor("M137",  // MQ-137 - Ammonia
      SENSOR_ACCUM_RATE, A7, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));
  }
}

// Initializes arduino pins
void DiyAirSensor::initPins() {
  pinMode(MENU_SELECT_BUTTON, INPUT_PULLUP);
  pinMode(MENU_UP_BUTTON,     INPUT_PULLUP);
  pinMode(MENU_DN_BUTTON,     INPUT_PULLUP);
}

// log all data to SD
void DiyAirSensor::log_sd(uint32_t id, uint32_t timestamp) {
  this->logfile->open_line(id, timestamp);
  this->logfile->write_field(this->sensor_state.device.num_sensors);
  for (int i = 0; i < this->sensor_state.device.num_sensors; i++) {
    this->logfile->write_field(this->sensor_state.sensor[i].data.value);
    this->logfile->write_field(this->sensor_state.sensor[i].data.raw);
    this->logfile->write_field(this->sensor_state.sensor[i].data.avg);
  }
  this->logfile->write_field((uint16_t)SD_FORMAT_VERSION);
  this->logfile->write_field("|");
  this->logfile->close_line();
}

// Called continuously in a loop
void DiyAirSensor::loop() {
  loop_start_millis = this->getMillis();
  this->loop_number++;

  // Before anything else, collect sensor data for most-precise timing.
  sensors->sense_all(&sensor_state);

  // Serial log loop start and sensor data
  Serial.print(loop_number++);
  Serial.print(F(" ----------- "));
  Serial.println(loop_start_millis);
  sensors->log_all_serial_only();

  // Output to the display
  sensor_display->display_data(&sensor_state);

  // todo: figure out why we rotate the file so often

  // Wrap the file every day
  if (this->getMillis() < this->day_start_millis ||  // millis wraps every 49.7d
    (this->getMillis() - this->day_start_millis)/1000 > SECONDS_PER_DAY) {
    this->logfile->rotate_file();
    this->day_start_millis = this->getMillis();
  }

  // Log to file every N loops, if file logging is configured on
  if (sensor_state.device.logging_enabled) {
    if (loop_number % sensor_state.device.log_every_n_loops == 0) {
      this->log_sd(this->loop_number, loop_start_millis);
    }
  }

  // Burn remainder of the loop period
  this->waitForSamplingPeriodEnd();
}

uint32_t DiyAirSensor::getMillis() {
  return millis();  // Arduino millis call
}

void DiyAirSensor::waitForSamplingPeriodEnd() {
  while (this->getMillis() < loop_start_millis +
        sensor_state.device.settings.data.sampling_period_ms) {
    // Check that millis() hasn't wrapped
    if (loop_start_millis > this->getMillis()) {
      // millis have wrapped - Should happen every 50 days, give or take
      loop_start_millis = this->getMillis();  // hacky
      break;
    }
  }
}
