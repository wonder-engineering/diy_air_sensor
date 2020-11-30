cmake_minimum_required(VERSION 3.12...3.16)
project(arduino-mock-test-all
				VERSION 0.0.1
				DESCRIPTION "test for the air sensor"
				LANGUAGES C CXX)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest
				EXCLUDE_FROM_ALL)

set(TARGET_NAME test-all)

# List all object files needed to build for test here.
#  Note: with the GLOB, we may not pick up sources in This
#  directory automatically without tweaking this file.
file(GLOB SRCS "*.cpp"
               "../src/Sd_i.cpp"
			   "../src/LogFile.cpp"
			   "../src/SensorMenu.cpp"
			   "../src/SensorArray.cpp"
			   "../src/Sensor.cpp"
			   "../src/MQSensor.cpp"
			   "../src/SensorSettings.cpp"
			   "../src/DiyAirSensor.cpp"
			   "../src/GP2YSensor.cpp"
			   "../src/AirSensorDisplay.cpp"
               "Stub_Serial.h")  # because it has no cpp file
add_executable(${TARGET_NAME} ${SRCS})

target_link_libraries(${TARGET_NAME}
		gtest_main
		gmock_main
)

# code coverage

set(CMAKE_BUILD_TYPE "coverage")

# Set tools paths appropriately for mac OSX or Linux builds
message("System name is ${CMAKE_SYSTEM_NAME}")
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(LLVM_COV_TOOLS_PATH "/Library/Developer/CommandLineTools/usr/bin")
    set(LLVM_COV "${LLVM_COV_TOOLS_PATH}/llvm-cov")
    set(LLVM_PROFDATA "${LLVM_COV_TOOLS_PATH}/llvm-profdata")
ELSE()
    set(LLVM_COV "llvm-cov")
    set(LLVM_PROFDATA "llvm-profdata")
ENDIF()

if(CMAKE_BUILD_TYPE STREQUAL "coverage" OR CODE_COVERAGE)
    if("${CMAKE_C_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
        message("Building with llvm Code Coverage Tools")

        # Warning/Error messages
        if(NOT LLVM_COV)
            message(FATAL_ERROR "llvm-cov not found at ${LLVM_COV_TOOLS_PATH}/llvm-cov! Aborting.")
        endif()

        # set Flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fprofile-instr-generate -fcoverage-mapping")

    elseif(CMAKE_COMPILER_IS_GNUCXX)
        message("Building with lcov Code Coverage Tools")

        # Warning/Error messages
        if(NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
            message(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
        endif()
        if(NOT LCOV_PATH)
            message(FATAL_ERROR "lcov not found! Aborting...")
        endif()
        if(NOT GENHTML_PATH)
            message(FATAL_ERROR "genhtml not found! Aborting...")
        endif()

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    else()
        message(FATAL_ERROR "Code coverage requires Clang or GCC. Aborting.")
    endif()
endif()

# llvm-cov
add_custom_target(${TARGET_NAME}-ccov-preprocessing
    COMMAND LLVM_PROFILE_FILE=${TARGET_NAME}.profraw "coverage:${TARGET_NAME}"
    COMMAND llvm-profdata merge -sparse ${TARGET_NAME}.profraw -o ${TARGET_NAME}.profdata
    DEPENDS ${TARGET_NAME})

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND LLVM_PROFILE_FILE=${TARGET_NAME}.profraw $<TARGET_FILE:${TARGET_NAME}>
    COMMAND ${LLVM_PROFDATA} merge -sparse ${TARGET_NAME}.profraw -o ${TARGET_NAME}.profdata
    COMMAND ${LLVM_COV} report $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata
    COMMAND ${LLVM_COV} show $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata -show-line-counts-or-regions > /dev/null
    COMMAND ${LLVM_COV} show $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata -show-line-counts-or-regions -output-dir=../coverage/${TARGET_NAME}-llvm-cov -format="html";
    COMMENT "Open coverage/${TARGET_NAME}-llvm-cov/index.html in your browser to view the coverage report."
)

add_dependencies(${TARGET_NAME} gtest_main gmock_main)
add_definitions(-DIN_TESTING)

target_compile_features(${TARGET_NAME} PUBLIC cxx_std_11)
enable_testing()
add_test(TestAll ${TARGET_NAME})
