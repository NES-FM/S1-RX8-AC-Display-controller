#pragma once
#include <Arduino.h>

#include "data_types.h"
#include "logger.h"

#include "Wire.h"

class clock {
  public:
    clock() {}
    void init();
    void tick( bool twentyfourhour, bool force );

    timeObj t;

    void setDS3231time( uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth,
                        uint8_t month, uint8_t year );
    uint8_t nowHour = 0, nowMinute = 0, nowSecond = 0, dayOfWeek = 0, dayOfMonth = 0, month = 0, year = 0;

  private:
    uint8_t prevMinute = 0;
    unsigned long nowTimeCycle = 0;

    bool enabled = false;

    static const uint8_t DS3231_I2C_ADDRESS = 0x68;

    uint8_t decToBcd( uint8_t val );
    uint8_t bcdToDec( uint8_t val );

    void readDS3231time();
};
