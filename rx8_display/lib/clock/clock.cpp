#include "clock.hpp"

void clock::init() 
{
    rtc = new RTC_DS1307();

    rtc->begin();
    //rtc.adjust(DateTime(2014, 1, 21, 4, 33, 0)); //a fixed time
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // your PC's time. ***** You may need to comment out after 1 upload to set the time - see wiki*******
    DateTime now = rtc->now();
    nowHour = now.hour();
    nowMinute = now.minute();
}

void clock::tick(bool twentyfourhour, bool force) 
{
    if (millis() - nowTimeCycle >= 10000 || force) // 'DateTime now' takes over 1 milliSecond to return a result, so we don't run it every cycle.
    {
        DateTime now = rtc->now();
        nowTimeCycle = millis();
        nowHour = now.hour();
        nowMinute = now.minute();

        if (!twentyfourhour && nowHour > 12) // 12 or 24 hour
        t.curHour = nowHour - 12;
        else
            t.curHour = nowHour;

        t.curMinute = nowMinute;

        //If the hour or minute are continually updating the other icons flicker\vanish as they are not set constantly, so we flag a minute change to manage display updates.
        t.minuteChange = t.prevMinute != t.curMinute;
        t.prevMinute = t.curMinute;
    }
}
