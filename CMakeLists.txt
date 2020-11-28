# Copyright Brett Leedy 2020

# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.1...3.16)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  diy_air_sensor
  VERSION 1.0
	LANGUAGES C CXX)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

# Embedded board options Adjust to your board
# Note, need to re-run cmake to get serial path.  This can be improved.
execute_process (
    COMMAND bash -c "if ls /dev/cu.wchusbserial*; then ls /dev/cu.wchusbesrial*; else echo /dev/null; fi"
		OUTPUT_VARIABLE SERIAL_PATH
)
set(MCU "atmega328p" CACHE STRING "Processor Type")
set(CPU_SPEED "16000000" CACHE STRING "Speed of the CPU")
set(PORT $SERIAL_PATH CACHE STRING "USB Port")
set(PORT_SPEED "115200" CACHE STRING "Serial Port Speed")
set(PIN_VARIANT "standard" CACHE STRING "Pin Selection Variant. Either standard, mega, leonardo, eightanaloginputs")
set(ARDUINO_PATH "/Applications/Arduino.app/Contents/Java" CACHE PATH "Path to arduino installation (Mac)" FORCE)
set(PROGRAMMER "stk500v2" CACHE STRING "Programmer Type")
set(COMPILE_FLAGS "" CACHE STRING "Additional Compiler Flags")

# Set own source files
# Simply list all your C / C++ source (not header!) files here
set(SRC_FILES src/${PROJECT_NAME}.cpp
				src/LogFile.cpp
				src/SensorMenu.cpp
				src/SensorArray.cpp
				src/Sd_i.cpp
				src/AirSensorDisplay.cpp
				src/DiyAirSensor.cpp
				src/GP2YSensor.cpp
				src/MQSensor.cpp
				src/Sensor.cpp
				src/SensorSettings.cpp
				)

add_subdirectory(cmake-modules
				EXCLUDE_FROM_ALL)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake-modules")

# Get cmake to recognize *.ino files as C++
# todo: it's not as simple as this. maybe fix up later
# set_source_files_properties(${PROJECT_NAME}.ino PROPERTIES LANGUAGE CXX)

# Find Arduino SDK delivered include directories
file(GLOB_RECURSE _TF2 ${ARDUINO_PATH}/**/math.h)
get_filename_component(AR_AVR_INC ${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino/Arduino.h DIRECTORY)
get_filename_component(AR_AVRGCC_INC ${_TF2} DIRECTORY)
message(STATUS "AR_AVR_INC: ${AR_AVR_INC}")
message(STATUS "PIN_VARIANT: ${ARDUINO_PATH}/hardware/arduino/ave/variants/${PIN_VARIANT}")

# Include directories
# todo: move all of these Arduino-IDE installed libs and sources to
#         submodules
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${ARDUINO_PATH}/hardware/arduino/cores/arduino
	${ARDUINO_PATH}/hardware/arduino/avr/variants/${PIN_VARIANT}
	${AR_AVR_INC}
	${AR_AVRGCC_INC}
	# additional libraries here
	${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI/src        # SPI
	${ARDUINO_PATH}/libraries/SD/src/                             # SD
	${ARDUINO_PATH}/hardware/arduino/avr/libraries/EEPROM/src/    # EEPROM
	${ARDUINO_PATH}/hardware/arduino/avr/libraries/Wire/src/      # Wire
	/Users/bmleedy/Documents/Arduino/libraries/LiquidCrystal_I2C/ # LCD
	)

# Find Arduino source files.
file(GLOB_RECURSE ARDUINO_CORE_SRC
	${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino/*.c
	${ARDUINO_PATH}/hardware/arduino/avr/cores/arduino/*.cpp
	)
# message("ARDUINO_CORE_SRC: ${ARDUINO_CORE_SRC}")

# Find Arduino library source files to build.
file(GLOB_RECURSE ARDUINO_LIB_SRC
	${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI/src/*.c       # SPI
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI/src/*.cc      # SPI
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/SPI/src/*.cpp     # SPI
  ${ARDUINO_PATH}/libraries/SD/src/*.c                             # SD
  ${ARDUINO_PATH}/libraries/SD/src/*.cc                            # SD
  ${ARDUINO_PATH}/libraries/SD/src/*.cpp                           # SD
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/EEPROM/src/*.c    # EEPROM
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/EEPROM/src/*.cc   # EEPROM
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/EEPROM/src/*.cpp  # EEPROM
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/Wire/src/*.c      # Wire
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/Wire/src/*.cc     # Wire
  ${ARDUINO_PATH}/hardware/arduino/avr/libraries/Wire/src/*.cpp    # Wire
  /Users/bmleedy/Documents/Arduino/libraries/LiquidCrystal_I2C/*.c   # LCD
  /Users/bmleedy/Documents/Arduino/libraries/LiquidCrystal_I2C/*.cc  # LCD
  /Users/bmleedy/Documents/Arduino/libraries/LiquidCrystal_I2C/*.cpp # LCD
	)
# message(STATUS "ARDUINO_LIB_SRC: ${ARDUINO_LIB_SRC}")

set(CMAKE_SYSTEM_NAME Arduino)

# Find Arduino SDK delivered programs
find_program(AR_AVR_GCC NAMES avr-gcc PATHS ${ARDUINO_PATH}/hardware/tools/avr/bin NO_DEFAULT_PATH)
find_program(AR_AVR_CXX NAMES avr-g++ PATHS ${ARDUINO_PATH}/hardware/tools/avr/bin NO_DEFAULT_PATH)
find_program(AR_AVR_OBJCOPY NAMES avr-objcopy PATHS ${ARDUINO_PATH}/hardware/tools/avr/bin NO_DEFAULT_PATH)
find_program(AR_AVR_OBJDUMP NAMES avr-objdump PATHS ${ARDUINO_PATH}/hardware/tools/avr/bin NO_DEFAULT_PATH)
find_program(AR_AVR_RANLIB NAMES avr-ranlib PATHS ${ARDUINO_PATH}/hardware/tools/avr/bin NO_DEFAULT_PATH)
find_program(AR_AVR_LD NAMES avr-ld PATHS ${ARDUINO_PATH}/hardware/tools/avr/bin NO_DEFAULT_PATH)

# Compiler suite specification
#  Need to specify this because we're cross-compiling
set(CMAKE_C_COMPILER ${AR_AVR_GCC})
set(CMAKE_CXX_COMPILER ${AR_AVR_CXX})
set(CMAKE_OBJCOPY ${AR_AVR_OBJCOPY})
set(CMAKE_OBJDUMP ${AR_AVR_OBJDUMP})
set(CMAKE_RANLIB ${AR_AVR_RANLIB})
set(CMAKE_LINKER ${AR_AVR_LD})

# Debugging verbosity
message(STATUS "CMAKE_C_COMPILER ${CMAKE_C_COMPILER}")

# Compiler flags
add_definitions(-mmcu=${MCU} -DF_CPU=${CPU_SPEED})
add_definitions(-c -g -Os -Wall)
add_definitions(-fno-exceptions -ffunction-sections -fdata-sections)
add_definitions(-DARDUINO=110 -DBUILD_WITH_CMAKE)  # For wire library, usually comes from IDE, I guess

# Linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")   # remove -rdynamic for C
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # remove -rdynamic for CXX
set(CMAKE_EXE_LINKER_FLAGS "-Os -Wl,--gc-sections -mmcu=${MCU}")

# Here's where we specify all of the source files we build/link with
add_executable(${PROJECT_NAME} ${ARDUINO_CORE_SRC} ${ARDUINO_LIB_SRC} ${SRC_FILES})

# Below here, we create a separate command to do the upload to the board
#  using avrdude
find_program(AR_AVRDUDE NAMES avrdude PATHS ${ARDUINO_PATH}/hardware/tools/ NO_DEFAULT_PATH)
file(GLOB_RECURSE AR_AVRDUDE_CFG ${ARDUINO_PATH}/**/avrdude.conf)

add_custom_target(download
	COMMAND ${CMAKE_OBJCOPY} -j .text -j .data -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
	COMMAND ${AR_AVRDUDE} -C${AR_AVRDUDE_CFG} -F -p${MCU} -c${PROGRAMMER} -P${PORT} -b${PORT_SPEED} -D -Uflash:w:${PROJECT_NAME}.hex:i
	DEPENDS ${PROJECT_NAME}
	)
