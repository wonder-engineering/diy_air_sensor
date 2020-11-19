cmake_minimum_required(VERSION 3.12...3.16)
project(arduino-mock-test-all
				VERSION 0.0.1
				DESCRIPTION "test for the air sensor"
				LANGUAGES C CXX)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest
				EXCLUDE_FROM_ALL)

file(GLOB SRCS "*.cpp" "../Sd_i.cpp" "../LogFile.cpp" "../SensorMenu.cpp" "../SensorArray.cpp" "AnalogSensor_test.cpp" "../Sensor.cpp" "../MQSensor.cpp")
add_executable(test-all ${SRCS})

target_link_libraries(test-all
		gtest_main
		gmock_main
)

add_dependencies(test-all gtest_main gmock_main)
add_definitions(-DIN_TESTING)

target_compile_features(test-all PUBLIC cxx_std_11)
enable_testing()
add_test(TestAll test-all)
