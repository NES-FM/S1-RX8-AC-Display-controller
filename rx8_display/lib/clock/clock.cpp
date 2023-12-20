#include "clock.hpp"

void clock::init() 
{
    rtc = new RTC_DS1307();

    if (rtc->begin()) {
        debugln("RTC Initialized");
        enabled = true;
    }
    else {
        debugln("RTC INIT FAILED!");
        enabled = false;
    }

    if (enabled) {
        //rtc.adjust(DateTime(2014, 1, 21, 4, 33, 0)); //a fixed time
        // rtc->adjust(DateTime(F(__DATE__), F(__TIME__))); // your PC's time. ***** You may need to comment out after 1 upload to set the time - see wiki*******
        DateTime now = rtc->now();
        nowHour = now.hour();
        nowMinute = now.minute();
        nowSecond = now.second();
        logln("Clock Initialized and adjusted to: %d:%d:%d. Clock %s", (int)nowHour, (int)nowMinute, (int)nowSecond, rtc->isrunning() ? "is running." : "is NOT running.");
    }
}

void clock::tick(bool twentyfourhour, bool force) 
{
    if (enabled) {
        if (millis() - nowTimeCycle >= 10000 || force) // 'DateTime now' takes over 1 milliSecond to return a result, so we don't run it every cycle.
        {
            DateTime now = rtc->now();
            nowTimeCycle = millis();
            nowHour = now.hour();
            nowMinute = now.minute();
            nowSecond = now.second();
            logln("Checking for time: %d:%d:%d. Clock %s", (int)nowHour, (int)nowMinute, (int)nowSecond, rtc->isrunning() ? "is running." : "is NOT running.");

            if (!twentyfourhour && nowHour > 12) // 12 or 24 hour
                t.curHour = nowHour - 12;
            else
                t.curHour = nowHour;

            //If the hour or minute are continually updating the other icons flicker\vanish as they are not set constantly, so we flag a minute change to manage display updates.
            if (prevMinute != nowMinute) {
                prevMinute = nowMinute;
                t.minuteChange = true;
                t.curMinute = nowMinute;
            }
        }
    }
}
