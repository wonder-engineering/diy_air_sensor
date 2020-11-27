// Copyright 2020 Brett M Leedy


#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "../Sensor.hh"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Mock;
using ::testing::A;
using ::testing::Gt;
using ::testing::Lt;

#define SANE_GAIN          10.0
#define SANE_ACCUM_RATE   0.1
#define SANE_ZERO_ADJUST   100
#define FAKE_SENSOR_READING 876
#define SANE_PIN            0

TEST(Sensor, sense) {
    class InstrumentedSensor : public Sensor {
     public:
        InstrumentedSensor(const char * short_name,
                    float accum_rate,
                    uint8_t pin,
                    uint16_t zero_adjust,
                    float gain) : Sensor {short_name, accum_rate,
                                           pin, zero_adjust, gain} {}
        MOCK_METHOD(uint16_t, read_sensor, (), (override));
    };

    InstrumentedSensor sensor("TEST", SANE_ACCUM_RATE,
        SANE_PIN, SANE_ZERO_ADJUST, SANE_GAIN);
    // Should call the implementation of read_sensor
    EXPECT_CALL(sensor, read_sensor()).WillRepeatedly(
                        Return(FAKE_SENSOR_READING));
    // should return the value returned by the impl
    ASSERT_EQ(FAKE_SENSOR_READING, sensor.sense());
    // should return the value of last reading from raw
    ASSERT_EQ(FAKE_SENSOR_READING, sensor.get_last_raw());
    // should properly compute values
    ASSERT_EQ(FAKE_SENSOR_READING * SANE_GAIN + SANE_ZERO_ADJUST,
                sensor.get_last_value());
    // should return something other than zero or last value for average
    ASSERT_NE(FAKE_SENSOR_READING, sensor.get_sensor_avg());
    ASSERT_NE(0, sensor.get_sensor_avg());
}
