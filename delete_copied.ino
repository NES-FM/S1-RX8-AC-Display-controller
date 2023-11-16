

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

bool acAmpCentigrade = true;


void setup() {
  

}

void loop()
{

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

