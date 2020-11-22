#include "DiyAirSensor.hh"

DiyAirSensor::DiyAirSensor() {
  this->loop_number = 0;
  this->loop_start_millis = 0;

  // Init Serial port
  Serial.begin(115200);
  while (!Serial); // Wait for it to be initialized.

  Serial.println(F("Init Button Pins..."));
  pinMode(MENU_SELECT_BUTTON, INPUT_PULLUP);
  pinMode(MENU_UP_BUTTON,     INPUT_PULLUP);
  pinMode(MENU_DN_BUTTON,     INPUT_PULLUP);

  Serial.println(F("Init Logfile..."));
  logfile = new LogFile();

  Serial.println(F("Init Sensors..."));
  sensors = new SensorArray();
  if(!sensor_state.device.settings.data.alt_sensor_config){
    sensors->add_sensor(new MQSensor(" LPG", SENSOR_ACCUM_RATE, A1, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ5 - LPG, City Gas Leak
    sensors->add_sensor(new MQSensor("  CO", SENSOR_ACCUM_RATE, A6, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ7 - Carbon Monoxide
    sensors->add_sensor(new MQSensor("Ozon", SENSOR_ACCUM_RATE, A7, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ131 - Ozone
    sensors->add_sensor(new MQSensor(" Gas", SENSOR_ACCUM_RATE, A3, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ9 Gas leaks
    sensors->add_sensor(new MQSensor(" Haz", SENSOR_ACCUM_RATE, A2, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ135Poison Gasses (organic)
    sensors->add_sensor(new GP2YSensor("Dust", SENSOR_ACCUM_RATE, A0, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN, 4)); // PM2.5 sensor
  } else {
    sensors->add_sensor(new MQSensor("MQ2 ", SENSOR_ACCUM_RATE, A0, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ2 - smoke
    sensors->add_sensor(new MQSensor("MQ7 ", SENSOR_ACCUM_RATE, A1, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ7 - Carbon Monoxide
    sensors->add_sensor(new MQSensor("MQ4 ", SENSOR_ACCUM_RATE, A2, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ4 - Methane (CNG)
    sensors->add_sensor(new MQSensor("MQ6 ", SENSOR_ACCUM_RATE, A3, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ-6 - LPG, iso-butane, propane
    sensors->add_sensor(new MQSensor("MQ8 ", SENSOR_ACCUM_RATE, A6, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ-8 - Hydrogen
    sensors->add_sensor(new MQSensor("M137", SENSOR_ACCUM_RATE, A7, DEFAULT_ZERO_ADJUST, DEFAULT_GAIN));  // MQ-137 - Ammonia
  }

  Serial.println(F("Init Configs..."));
  sensors->write_sensor_configs(&sensor_state);
  Serial.print(F("sensor 0 shorname: ")); Serial.println(sensor_state.sensor[0].config.shortname);

  Serial.println(F("Init LCD..."));
  sensor_display = new AirSensorDisplay();

  Serial.println(F("Init done."));
}



void DiyAirSensor::loop() {
  loop_start_millis = millis();

  // Before anything else, collect sensor data for most-precise timing.
  sensors->sense_all(&sensor_state);

  // Serial log loop start and sensor data
  Serial.print(loop_number++);
  Serial.print(F(" ----------- "));
  Serial.println(loop_start_millis);
  sensors->log_all_serial_only();

  // Output to the display
  sensor_display->display_data(&sensor_state);
//
//  // Wrap the file every day
//  if(loop_number % (SECONDS_PER_DAY / (sensor_state.device.sampling_period_ms / 1000)) == 0){
//    Serial.println(F("Wrapping the log File"));
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
  while(millis() < loop_start_millis + sensor_state.device.settings.data.sampling_period_ms) {
    // Check that millis() hasn't wrapped
    if(loop_start_millis > millis()){
      //millis have wrapped - Should happen every 50 days, give or take
      loop_start_millis = millis(); //hacky
      break;
    }

  }// while(millis)
}
