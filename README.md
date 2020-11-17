# diy_air_sensor
This is the project for the DIY Air Quality Sensor.  The code right now is, frankly, atrocious.  There are things I did in there that I don't care to remember, but they haunt me when I close my eyes.  This project started as a quick-and-hacky sensor project and morphed one-device-by-one-device into something akin to a coral colony, or maybe a cancer.  Anyway, whatever you do, DO NOT USE THIS CODE.  It will scare your children and sleep with your wife.

Bit-by-bit, I'm going to build some tests then start refactoring.  I'll remove this readme-of-shame when I have something resembling real code.

# TODO List:
* Write unit tests
* Make unit tests actually run in cmake environment
* Factor out classes sharing static dependencies
* Factor out reliance on side effects
* Use modern STL constructs for lists
* DRY-ify code
* use clang-tidy
* consistent formatting
* obey the linter
* linter as automatic tests
* code coverage in testing infra
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
* Fix logging to card - slow down SPI bus?
