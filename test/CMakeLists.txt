cmake_minimum_required(VERSION 3.12...3.16)
project(arduino-mock-test-all
				VERSION 0.0.1
				DESCRIPTION "test for the air sensor"
				LANGUAGES C CXX)



find_package(Threads REQUIRED)
# add_subdirectory(arduino_mock)

include_directories(
				# ${ARDUINO_MOCK_INCLUDE_DIRS}
		googletest/googlemock/include
		googletest/googletest/includ
		${CMAKE_CURRENT_SOURCE_DIR}
)


# message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# file(
#				GLOB_RECURSE GTEST_SRC
#				googletest/*.cc
#				googletest/*.cpp
#				googletest/*.c
#  )
#message(STATUS "GTEST_SRC: ${GTEST_SRC}")

# add_subdirectory(googletest)
# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest
				EXCLUDE_FROM_ALL)



file(GLOB LIB_SRCS "../*.ino")
file(GLOB SRCS "*.cpp")
add_executable(test-all ${SRCS} ${LIB_SRCS} ${GTEST_SRC})


target_link_libraries(test-all
				# ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest-build/googlemock/gtest/libgtest.a
				# ${ARDUINO_MOCK_LIBS_DIR}/lib/gtest/gtest/src/gtest-build/googlemock/libgmock.a
				# ${ARDUINO_MOCK_LIBS_DIR}/dist/lib/libarduino_mock.a
				# ${CMAKE_THREAD_LIBS_INIT}
		gtest_main
		gmock_main
)

add_dependencies(test-all gtest_main gmock_main)
add_definitions(-DIN_TESTING)

target_compile_features(test-all PUBLIC cxx_std_11)
enable_testing()
add_test(TestAll test-all)
