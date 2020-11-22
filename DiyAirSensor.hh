#ifndef DIY_AIR_SENSOR_HH_
#define DIY_AIR_SENSOR_HH_


#include "Arduino.h"

#include "SensorArray.hh"
#include "LogFile.h"
#include "GP2YSensor.hh"
#include "MQSensor.hh"
#include "AirSensorDisplay.hh"


/*------------------------------------
 * Gas sensor info
 *
 * MQ-2 - Smoke -
 * MQ-3 - Alcohol Gas - https://www.sparkfun.com/products/8880
 * MQ-4 - Mehane (CNG) - https://www.sparkfun.com/products/9404
 * MQ-5 - LPG, natural gas, town gas - https://tinyurl.com/y464ghh8
 * MQ-6 - LPG, iso-butane, propane - https://tinyurl.com/y4pnkfsl
 * MQ-7 - Carbon Monoxide (CO) - https://tinyurl.com/zr2uqtn
 * MQ-8 - Hydrogen - https://www.sparkfun.com/products/10916
 * MQ-9 - Gas leaks - https://wiki.seeedstudio.com/Grove-Gas_Sensor-MQ9/
 * MQ-131 - Ozone - https://www.sparkfun.com/products/17051
 * MQ-135 - Harmful Gasses https://components101.com/sensors/mq135-gas-sensor-for-air-quality
 * MQ-136 - Hydrogen Sulfide - https://www.sparkfun.com/products/17052 (spare one in yellow box)
 * MQ-137 - Ammonia - https://www.sparkfun.com/products/17053
 * Multi-channel gas sensor: https://wiki.seeedstudio.com/Grove-Multichannel_Gas_Sensor/
 */

// Constants
#define SECONDS_PER_DAY      86400
#define SENSOR_ACCUM_RATE    0.1
#define DEFAULT_ZERO_ADJUST  0
#define DEFAULT_GAIN         1.0



class DiyAirSensor {
  public:
    DiyAirSensor();
    void loop();

  private:
    // Classes for main components
    SensorState       sensor_state;  // one "database" for stateful data
    LogFile           * logfile;
    SensorArray       * sensors;     // container for all the sensors
    AirSensorDisplay  * sensor_display;
    uint32_t loop_number;
    uint32_t loop_start_millis;
};

  #endif
