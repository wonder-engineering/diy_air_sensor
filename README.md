# diy_air_sensor
This is the project for the DIY Air Quality Sensor.  The code right now is, frankly, atrocious.  There are things I did in there that I don't care to remember, but they haunt me when I close my eyes.  This project started as a quick-and-hacky sensor project and morphed one-device-by-one-device into something akin to a coral colony, or maybe a cancer.  Anyway, whatever you do, DO NOT USE THIS CODE.  It will scare your children and sleep with your wife.

Bit-by-bit, I'm going to build some tests then start refactoring.  I'll remove this readme-of-shame when I have something resembling real code.

# TODO List:
* Diagnose and fix SD writing problems (possibly hardware)
* Make unit tests actually run in cmake environment
* use clang-tidy
* linter as automatic tests
* code coverage in testing infra (https://dr-kino.github.io/2019/12/22/test-coverage-using-gtest-gcov-and-lcov/)
* Make portable to anywhere but my own personal mac
* Auto lint with build command
* Make testig cmake more elegant
* Move library build dependencies to git submodules
* Integrate with Eclipse or preferred IDE
* Portable to linux dev environment
* Cross compilation tools in git submodules
* Travis CI integration test
* Clang fuzz tests
* Clang tidy integration
* Clang static analysis
* Clean up build warnings
* BOM for gas sensor purchase parts
* link enclosure files here
* testing/assembly instructions in this project.
* Calibration instructions
* move files to src/ and include/ directories for more tidy repo
asj-023yasl;S


# Gas sensor info
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
