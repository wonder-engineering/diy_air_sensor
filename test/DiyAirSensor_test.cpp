// Copyright 2020 Brett M Leedy

#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../src/DiyAirSensor.hh"

using ::testing::AtLeast;
using ::testing::Between;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;
using ::testing::InSequence;


#define SANE_TEST_ID       19
#define SANE_GAIN          10.0
#define SANE_LCD_HEIGHT     4
#define SANE_ACCUM_RATE   0.1
#define SANE_ZERO_ADJUST   100
#define LCD_FIRST_ROW       0



// Init instantiates everything we expect
// 6 sensors are added
// particle sensor is called once
// sensors are added
// config is loaded from sensors
// init will only run once

TEST(DiyAirSensor, init) {
    class MockedSerial : public HardwareSerial {
     public:
        MOCK_METHOD(uint8_t, begin, (uint32_t));
    } instrumentedSerial;

    class MockedSensorArray : public SensorArray {
     public:
        MOCK_METHOD(bool, add_sensor, (Sensor * newsens), (override));
    } sensors;

    // this instumented class will not call init() in the constructor
    class InstrumentedDiyAirSensor : public DiyAirSensor {
     public:
        InstrumentedDiyAirSensor(HardwareSerial * serial, SensorArray * sensors) :
            DiyAirSensor{serial, false} {
          // override internally-initialized array with mocked version.
          delete this->sensors;
          this->sensors = sensors;
        }
        MOCK_METHOD(void, initPins, (), (override));
        MOCK_METHOD(void, addSensors, (SensorArray * array), (override));
        MOCK_METHOD(void, updateConfigsFromSensor, (), (override));
    } airSensor(&instrumentedSerial, &sensors);

    // Expect all initializations to happen as expected
    EXPECT_CALL(airSensor, initPins());
    EXPECT_CALL(airSensor, addSensors(_));
    EXPECT_CALL(airSensor, updateConfigsFromSensor());
    EXPECT_CALL(instrumentedSerial, begin(115200));

    airSensor.init(&instrumentedSerial);

    // Test that nothing runs when called a second time
    EXPECT_CALL(airSensor, initPins()).Times(0);
    EXPECT_CALL(airSensor, addSensors(_)).Times(0);
    EXPECT_CALL(airSensor, updateConfigsFromSensor()).Times(0);
    EXPECT_CALL(instrumentedSerial, begin(115200)).Times(0);
    airSensor.init(&instrumentedSerial);
}


TEST(DiyAirSensor, addSensors) {
    class MockedSerial : public HardwareSerial {
     public:
        MOCK_METHOD(uint8_t, begin, (uint32_t));
    } instrumentedSerial;

    class MockedSensorArray : public SensorArray {
     public:
        MOCK_METHOD(bool, add_sensor, (Sensor * newsens), (override));
    } sensors;

    // this instumented class will not call init() in the constructor
    class InstrumentedDiyAirSensor : public DiyAirSensor {
     public:
        InstrumentedDiyAirSensor(HardwareSerial * serial, SensorArray * sensors) :
                DiyAirSensor{serial} {
            // override internally-initialized array with mocked version.
            delete this->sensors;
            this->sensors = sensors;
        }
        // expose private method for this test
        void addSensors(SensorArray * array) {DiyAirSensor::addSensors(array);}
        void set_alt_config() {
            sensor_state.device.settings.data.alt_sensor_config = true;
        }
    } airSensor(&instrumentedSerial, &sensors);

    // Expect at least one and not more than the max number of sensors
    EXPECT_CALL(sensors, add_sensor(_)).Times(
          Between(1, SENSORSTATE_MAX_NUM_SENSORS));
    airSensor.addSensors(&sensors);

    // Expect at least one and not more than the max number of sensors
    //   with alternate configuration
    EXPECT_CALL(sensors, add_sensor(_)).Times(
          Between(1, SENSORSTATE_MAX_NUM_SENSORS));
    airSensor.set_alt_config();
    airSensor.addSensors(&sensors);
}


TEST(DiyAirSensor, loop) {
    class MockedSerial : public HardwareSerial {
     public:
        MOCK_METHOD(uint8_t, begin, (uint32_t));
    } instrumentedSerial;

    class MockedSensorArray : public SensorArray {
     public:
        MOCK_METHOD(void, sense_all, (SensorState *), (override) );
        MOCK_METHOD(void, log_all_serial_only, (), (override) );
    } sensors;

    class MockedSensorDisplay :  public AirSensorDisplay {
     public:
        MOCK_METHOD(void, display_data,
                    (SensorState * sensor_state),
                    (override));
    } display;

    // this instumented class will not call init() in the constructor
    class InstrumentedDiyAirSensor : public DiyAirSensor {
     public:
        InstrumentedDiyAirSensor(HardwareSerial * serial,
                                SensorArray * sensors,
                                AirSensorDisplay * display) :
                                  DiyAirSensor{serial} {
            // override internally-initialized array
            delete this->sensors;
            this->sensors = sensors;
            delete this->sensor_display;
            this->sensor_display = display;
        }
        MOCK_METHOD(uint32_t, getMillis,
                        (long unsigned int(&millisFunc)()),
                        (override));
        MOCK_METHOD(void, waitForSamplingPeriodEnd, (long unsigned int(&)()), (override));
    } airSensor(&instrumentedSerial, &sensors, &display);


    // Calls must be in sequence
    InSequence s;

    EXPECT_CALL(airSensor, getMillis(_));
    EXPECT_CALL(sensors, sense_all(_));
    EXPECT_CALL(sensors, log_all_serial_only());
    EXPECT_CALL(display, display_data(_));
    EXPECT_CALL(airSensor, waitForSamplingPeriodEnd(_));

    airSensor.loop(&instrumentedSerial, millis);
}
