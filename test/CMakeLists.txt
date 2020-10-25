cmake_minimum_required(VERSION 3.12...3.16)
project(arduino-mock-test-all
				VERSION 0.0.1
				DESCRIPTION "test for the air sensor"
				LANGUAGES C CXX)



find_package(Threads REQUIRED)
add_subdirectory(arduino_mock)

add_subdirectory("googletest")

include_directories(
    ${ARDUINO_MOCK_INCLUDE_DIRS}
		googletest/googlemock/include
		googletest/googletest/include
)

file(GLOB LIB_SRCS "../*.ino")
file(GLOB SRCS "*.cpp")
add_executable(test-all ${SRCS} ${LIB_SRCS})

target_link_libraries(test-all
    ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest-build/googlemock/gtest/libgtest.a
    ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest-build/googlemock/libgmock.a
    ${ARDUINO_MOCK_LIBS_DIR}/dist/lib/libarduino_mock.a
    ${CMAKE_THREAD_LIBS_INIT}
)

add_dependencies(test-all arduino_mock)
add_definitions(-DIN_TESTING)

target_compile_features(test-all PUBLIC cxx_std_11)
enable_testing()
add_test(TestAll test-all)
