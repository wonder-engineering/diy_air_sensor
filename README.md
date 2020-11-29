# diy_air_sensor
This is the project for the DIY Air Quality Sensor.  The code right now is, frankly, atrocious.  There are things I did in there that I don't care to remember, but they haunt me when I close my eyes.  This project started as a quick-and-hacky sensor project and morphed one-device-by-one-device into something akin to a coral colony, or maybe a cancer.  Anyway, whatever you do, DO NOT USE THIS CODE.  It will scare your children and sleep with your wife.

Bit-by-bit, I'm going to build some tests then start refactoring.  I'll remove this readme-of-shame when I have something resembling real code.

# TODO List:
* Diagnose and fix SD writing problems (possibly hardware)
* Move Arduino library build dependencies to git submodules
* Clang fuzz tests
* Clang tidy integration
* Clang static analysis


# Gas sensor info
 * MQ-2 - Smoke
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

| item | Part Number | Quantity | link | Description               | Est Cost |
|------|-------------|----------|------|---------------------------|----------|
| 1    |  MQ5        |    1     |      |   LPG, City Gas Leak      |   $2     |
| 2    |  MQ7        |    1     |      |   Carbon Monoxide         |  $3.50   |
| 3    |  MQ131      |    1     |      |   Ozone                   |   $30    |
| 4    |  MQ9        |    1     |      |   Gas leaks               |   $2     |
| 5    |  MQ135      |    1     |      |   Poison Gasses (organic) |   $2     |
| 6    |GP2Y1010AU0F |    1     |      |   PM2.5 sensor            |  $18     |
| 7    |  Nano       |    1     |      |   Arduino Nano            |   $5     |
| 8    |   ---       |    1     |      |   F-F Breadboard cables   |   $6     |
| 9    |  ENCL       |    1     |      |   3d printed enclosure    |   $3     |
| 10   |  LK51-CA    |    1     |      |   4-line I2C LCD module   |   $12    |
| 11   |3-01-0038-5PCS|    1    |      |   SD Card module          |   $5    |

# 3D Printed Enclosure

See https://www.thingiverse.com/thing:4669454.

# Calibration

Once I have calibrated my own system, I will post instructions here.

# Assembly instructions

I may (or may not) add assembly instructions eventually
