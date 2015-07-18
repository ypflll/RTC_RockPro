# RTC_RockPro

RTC for Rockpro is used to configure the real-time clock(RTC) chip HYM8563 on a
RockPro board, which also can be used on other Radxa board.
HYM8563 is configured through I2C bus, so the modular GPIO interface wiringX
which provides easy I2C APIs should be installed firstly.
See here: https://github.com/wiringX/wiringX.

* When compiling, add the wiringX lib:

```
gcc -o example example.c /path/libwiringX.a
```

When board is shutdown, RTC still runs and outputs INT signal at the set time, 
which is connected to the PMU module. The PMU module can turn on the board automatically.
