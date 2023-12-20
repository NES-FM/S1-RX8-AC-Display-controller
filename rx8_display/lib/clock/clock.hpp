#pragma once
#include <Arduino.h>

#include "data_types.h"
#include "logger.h"

#include "RTClib.h"

class clock {
    public:
        clock() {}
        void init();
        void tick(bool twentyfourhour, bool force);

        timeObj t;
    private:
        RTC_DS1307* rtc;
        uint8_t nowHour = 0;
        uint8_t nowMinute = 0;
        uint8_t nowSecond = 0;
        uint8_t prevMinute = 0;
        unsigned long nowTimeCycle = 0;

        bool enabled = false;
};
