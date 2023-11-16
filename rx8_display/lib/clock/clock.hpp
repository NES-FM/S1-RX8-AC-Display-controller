#pragma once
#include <Arduino.h>

#include "data_types.h"

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
        unsigned long nowTimeCycle;
};
