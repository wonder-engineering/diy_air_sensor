// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../src/SensorArray.hh"
#include "../src/MQSensor.hh"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;


#define SANE_TEST_ID       19
#define SANE_GAIN          10.0
#define SANE_LCD_HEIGHT     4
#define SANE_ACCUM_RATE   0.1
#define SANE_ZERO_ADJUST   100
#define LCD_FIRST_ROW       0
#define FAKE_SENSOR_READING 876


TEST(SensorArray, add_sensor) {
    class InstrumentedSensorArray : public SensorArray {
     public:
        uint8_t get_num_sensors() {return SensorArray::get_num_sensors();}
        uint8_t get_sensor_avg(uint8_t sensor_id) {
            return SensorArray::get_sensor_avg(sensor_id);
        }
        uint8_t get_sensor_raw(uint8_t sensor_id) {
            return SensorArray::get_sensor_raw(sensor_id);
        }
        void get_short_name(uint8_t i, char buffer[], uint8_t buffer_size ) {
            SensorArray::get_short_name(i, buffer, buffer_size);
        }
    } sensor_array;

    char short_name_buffer[SENSOR_SHORT_NAME_LEN];
    // Successfully add the max number of sensors.
    for (int sensor_number = 0;
            sensor_number < SENSORSTATE_MAX_NUM_SENSORS;
            sensor_number++ ) {
        ASSERT_LE(snprintf(short_name_buffer, sizeof(short_name_buffer),
                            "S_%d", sensor_number),
                            SENSOR_SHORT_NAME_LEN);  // meets short name
        ASSERT_TRUE(sensor_array.add_sensor(new MQSensor(short_name_buffer,
                                SANE_ACCUM_RATE,
                                sensor_number, SANE_ZERO_ADJUST, SANE_GAIN)));
        ASSERT_EQ(sensor_array.get_num_sensors(), sensor_number+1);
        char compare_name_buffer[SENSOR_SHORT_NAME_LEN];
        sensor_array.get_short_name(sensor_number, compare_name_buffer,
                                    sizeof(short_name_buffer));
        ASSERT_STREQ(short_name_buffer, compare_name_buffer);

        // State should start zeroed always
        ASSERT_EQ(sensor_array.get_sensor_avg(sensor_array.get_num_sensors()-1),
                                SENSOR_DEFAULT_AVG_VALUE + SANE_ZERO_ADJUST);
        ASSERT_EQ(sensor_array.get_sensor_raw(sensor_array.get_num_sensors()-1),
                                                SENSOR_DEFAULT_LAST_VALUE);
    }

    // Confirm test filled up the sensors
    ASSERT_EQ(sensor_array.get_num_sensors(), SENSORSTATE_MAX_NUM_SENSORS);

    // I cannot add more sensors after the max:
    ASSERT_LE(snprintf(short_name_buffer, sizeof(short_name_buffer),
                    "S_%d", SANE_TEST_ID),
                    SENSOR_SHORT_NAME_LEN);  // short name requirement
    ASSERT_FALSE(sensor_array.add_sensor(new MQSensor(short_name_buffer,
                                SANE_ACCUM_RATE,
                                SANE_TEST_ID, SANE_ZERO_ADJUST, SANE_GAIN)));
    ASSERT_EQ(sensor_array.get_num_sensors(), SENSORSTATE_MAX_NUM_SENSORS);
}


// todo: sense_all
TEST(SensorArray, sense_all) {
    class MockMQSensor : public MQSensor {
     public:
        MockMQSensor(const char * short_name, float accum_rate,
               uint8_t analog_pin, uint16_t zero_adjust, float gain) :
               MQSensor{short_name, accum_rate,
                        analog_pin, zero_adjust, gain} {}
        MOCK_METHOD(uint16_t, read_sensor, (), (override));
    };

    class InstrumentedSensorArray : public SensorArray {
     public:
        uint8_t get_num_sensors() {return SensorArray::get_num_sensors();}
    };

    InstrumentedSensorArray sensor_array = InstrumentedSensorArray();

    char short_name_buffer[SENSOR_SHORT_NAME_LEN];
    MockMQSensor * mock_sensor[SENSORSTATE_MAX_NUM_SENSORS];
    // Successfully add the max number of sensors.
    for (int sensor_number = 0;
            sensor_number < SENSORSTATE_MAX_NUM_SENSORS;
            sensor_number++ ) {
        ASSERT_LE(snprintf(short_name_buffer,
                    sizeof(short_name_buffer), "S_%d", SANE_TEST_ID),
                    SENSOR_SHORT_NAME_LEN);  // test name meets requirement
        mock_sensor[sensor_number] = new MockMQSensor(short_name_buffer,
                    SANE_ACCUM_RATE,
                    sensor_number, SANE_ZERO_ADJUST, SANE_GAIN);
        testing::Mock::AllowLeak(mock_sensor[sensor_number]);
        sensor_array.add_sensor(mock_sensor[sensor_number]);
    }

    // Confirm test filled up the sensors
    ASSERT_EQ(sensor_array.get_num_sensors(), SENSORSTATE_MAX_NUM_SENSORS);
    // Expect each configured sensor to be called once
    for (int sensor_number = 0;
            sensor_number < SENSORSTATE_MAX_NUM_SENSORS;
            sensor_number++ ) {
        EXPECT_CALL(*mock_sensor[sensor_number],
                    read_sensor()).WillRepeatedly(Return(FAKE_SENSOR_READING));
    }

    SensorState state;
    sensor_array.sense_all(&state);

    for (int sensor_number = 0;
            sensor_number < SENSORSTATE_MAX_NUM_SENSORS;
            sensor_number++ ) {
        ASSERT_EQ(FAKE_SENSOR_READING, state.sensor[sensor_number].data.raw);
    }

    for (int sensor_number = 0;
            sensor_number < SENSORSTATE_MAX_NUM_SENSORS;
            sensor_number++ ) {
        delete mock_sensor[sensor_number];
    }
}
