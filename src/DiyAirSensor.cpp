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
DiyAirSensor::DiyAirSensor(HardwareSerial * serial, bool do_init) {
  if (do_init)
    this->init(serial);
}
void DiyAirSensor::init(HardwareSerial * serial) {
  if (initialized) {
    serial->println(F("WARNING: Initialization will only run once."));
    return;
  }

  this->loop_number = 0;        // state for loop control
  this->loop_start_millis = 0;

  // Init Serial port
  serial->begin(115200);

  serial->println(F("Init Button Pins..."));
  this->initPins();

  // serial->println(F("Init Logfile..."));
  // logfile = new LogFile();
  serial->println(F("Init Sensors..."));
  sensors = new SensorArray();
  this->addSensors(sensors);

  serial->println(F("Init Configs..."));
  this->updateConfigsFromSensor();

  serial->println(F("Init LCD..."));
  sensor_display = new AirSensorDisplay();

  serial->println(F("Init done."));
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

// Called continuously in a loop
void DiyAirSensor::loop(HardwareSerial * serial, long unsigned int(& millisFunc)()) {
  loop_start_millis = this->getMillis(millisFunc);

  // Before anything else, collect sensor data for most-precise timing.
  sensors->sense_all(&sensor_state);

  // Serial log loop start and sensor data
  serial->print(loop_number++);
  serial->print(F(" ----------- "));
  serial->println(loop_start_millis);
  sensors->log_all_serial_only();

  // Output to the display
  sensor_display->display_data(&sensor_state);
//
//  // Wrap the file every day
//  if(loop_number % (SECONDS_PER_DAY /
//        (sensor_state.device.sampling_period_ms / 1000)) == 0){
//    serial->println(F("Wrapping the log File"));
//  }
//
//  // Log to file every N loops, if file logging is configured on
//  if(sensor_state.device.logging_enabled){
//    if(loop_number % sensor_state.device.log_every_n_loops == 0) {
//      logfile->open_line(loop_number, loop_start_millis);
//      // fixme sensors->log_all(logfile->get_file_ptr());
//      logfile->close_line();
//    }
//  }
//
//  // Set sensor zeros, based on menu adjustments
//
  // Burn remainder of the loop period
  this->waitForSamplingPeriodEnd(millisFunc);
}

uint32_t DiyAirSensor::getMillis(long unsigned int(&millisFunc)()) {
  return millisFunc();  // Arduino millis call
}

void DiyAirSensor::waitForSamplingPeriodEnd(long unsigned int(&millisFunc)()) {
  while (this->getMillis(millisFunc) < loop_start_millis +
        sensor_state.device.settings.data.sampling_period_ms) {
    // Check that millis() hasn't wrapped
    if (loop_start_millis > this->getMillis(millisFunc)) {
      // millis have wrapped - Should happen every 50 days, give or take
      loop_start_millis = this->getMillis(millisFunc);  // hacky
      break;
    }
  }
}
