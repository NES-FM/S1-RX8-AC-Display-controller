#include <Wire.h>
//#include "DS3231.h"
#include "RTClib.h"

RTC_DS1307 rtc;
//RTC_DS3231 rtc;



// Menu
byte menuIndexMin = 0;
byte menuIndexMax = 255;
byte menuIndex = 0;
byte menuValueMin = 0;
byte menuValueMax = 255;
byte menuValue = 0;
bool menuStartValue = false;
bool setHourFirst = true;
bool inMenu = false;




//Data processing
byte previousMinute;
unsigned long nowTimeCycle;
byte nowHour = 0;
byte nowMinute = 0;
bool minuteChange = false;
bool acAmpCentigrade = true;
bool clock24Hour = true; //false = 12 hour clock





void setup() {
  



  // Real time clock
  rtc.begin();
  //rtc.adjust(DateTime(2014, 1, 21, 4, 33, 0)); //a fixed time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // your PC's time. ***** You may need to comment out after 1 upload to set the time - see wiki*******
  DateTime now = rtc.now();
  nowHour = now.hour();
  nowMinute = now.minute();


}

void loop()
{
  calculateTime();




}









/*****************************************************************
  // Time and temp
*****************************************************************/

void calculateTime()
{
  if (millis() - nowTimeCycle >= 10000 || confMode) // 'DateTime now' takes over 1 milliSecond to return a result, so we don't run it every cycle.
  {
    DateTime now = rtc.now();
    nowTimeCycle = millis();
    nowHour = now.hour();
    nowMinute = now.minute();
  }

  if (!clock24Hour && nowHour > 12) // 12 or 24 hour, for now set in main controls via clock24Hour
  {
    currentHour = nowHour - 12;
  }
  else
  {
    currentHour = nowHour;
  }

  currentMinute = nowMinute;



  //If the hour or minute are continually updating the other icons flicker\vanish as they are not set constantly, so we flag a minute change to manage display updates.
  minuteChange = (previousMinute != currentMinute) ? true : false;
  previousMinute = currentMinute;
}





/*****************************************************************
  // Menu
*****************************************************************/

void updateClockTime()
{
  inMenu = true;
  if (shortPushButton == 1)
  {
    setHourFirst = (!setHourFirst) ? true : false;
    menuStartValue = false;
  }

  if (setHourFirst && inMenu)
  {
    menuValueMax = (clock24Hour) ? 23 : 12;
    populateFixedLcdDisplay("Set Hour");
    menuValue = (!menuStartValue) ? currentHour : menuValue ;
    currentHour = menuValue;
    menuStartValue = true;
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), menuValue, now.minute(), now.second()));
  }

  if (!setHourFirst && inMenu)
  {
    menuValueMax = 59;
    populateFixedLcdDisplay("Set Minute");
    menuValue = (!menuStartValue) ? currentMinute : menuValue ;
    currentMinute = menuValue;
    menuStartValue = true;
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), menuValue, now.second()));

  }
}

void confMenu()
{
  menuIndexMin = 1; // Make sure we start at menu position 1.
  menuIndexMax = 3;
  inMenu = false;

  if (longPushButton == 8) // Roll back the menu
  {
    confMode = false;
    acAmpOn = true;
    acAmpRxChanged = true;
    acAmpMessageToProcess = true;
    menuStartValue = false;
    menuIndex = 1;
  }

  switch (menuIndex) // Also set subMenu and index options
  {
    case 1 : populateFixedLcdDisplay("Temp C or F"); if (shortPushButton == 1) acAmpCentigrade = (!acAmpCentigrade) ? true : false; iconArray[0][6] = (acAmpCentigrade) ? iconArray[0][6] += 0x10 : iconArray[0][6] += 0x08;
      break;
    case 2 : populateFixedLcdDisplay("24/12 Hour"); if (clock24Hour) {
        sevenSegmentCalculate(0x01, 2);
        sevenSegmentCalculate(0x04, 4);
      } else {
        sevenSegmentCalculate(0x01, 1);
        sevenSegmentCalculate(0x04, 2);
      } if (shortPushButton == 1) clock24Hour = (!clock24Hour) ? true : false;
      break;
    case 3 : menuIndexMax = 3; updateClockTime();
      break;
  }

  longPushButton = 0;
  shortPushButton = 0;
}

