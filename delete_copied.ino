#include <SPI.h>
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
byte currentTemp;
byte currentHour;
byte currentMinute;
byte previousMinute;
unsigned long nowTimeCycle;
byte nowHour = 0;
byte nowMinute = 0;
bool minuteChange = false;
bool acAmpCentigrade = true;
bool clock24Hour = true; //false = 12 hour clock

// iconArray for display icons 90-9D, first 5 LSB, calculated via addition. iconArray[0] = current, iconArray[1] = LCD display registry values.
byte iconArray[2][14] = {0};
byte sevenSegmentArray[1][7] = {0};


/**********************************************
// User configurable
**********************************************/
String welcome = "Test Beta3.0"; // populate something into the text display - 12 Char max anything else is truncated.


void setup() {
  // we use iconArray[1] to define the registers we write too with the data thats populated into iconArray[0]
  iconArray[1][0] = 0x90; iconArray[1][1] = 0x91; iconArray[1][2] = 0x92; iconArray[1][3] = 0x93; iconArray[1][4] = 0x94; iconArray[1][5] = 0x95; iconArray[1][6] = 0x96;
  iconArray[1][7] = 0x97; iconArray[1][8] = 0x98; iconArray[1][9] = 0x99; iconArray[1][10] = 0x9A; iconArray[1][11] = 0x9B; iconArray[1][12] = 0x9C; iconArray[1][13] = 0x9D;

  // Control pins for the display SPI
  pinMode(mode0, OUTPUT);
  pinMode(mode1, OUTPUT);
  pinMode(ssPin, OUTPUT);

  //Control panel LED's
  pinMode(rearDemistLED, OUTPUT);
  pinMode(frontDemistLED, OUTPUT);
  pinMode(freshAirLED, OUTPUT);
  pinMode(autoLED, OUTPUT);
  pinMode(AirConLED, OUTPUT);

  // Real time clock
  rtc.begin();
  //rtc.adjust(DateTime(2014, 1, 21, 4, 33, 0)); //a fixed time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // your PC's time. ***** You may need to comment out after 1 upload to set the time - see wiki*******
  DateTime now = rtc.now();
  nowHour = now.hour();
  nowMinute = now.minute();


  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3)); //I couldn't see a defined max speed in the NJU6623 datasheet, 10Meg is quite an increase on the original 9600, so need to monitor incase this is not stable.
 // Serial.begin (115200, SERIAL_8N1);
  

  delay(300);
  lcdDisplayStartUp(); // run the LCD default settings and clear the display icons

}

void loop()
{
  checkAndSendHardwareControl();
  calculateTime();

  if (!confMode)
  {
  
    if (acAmpMessageToProcess && acAmpRxChanged || minuteChange)
    {
      acAmpLedFuntions(acAmpOn);
      if (acAmpOn)
      {
        acAmpRecievedStatus();

        acAmpProcessAC();
        acAmpProcessFan();
        acAmpProcessTemp();
        acAmpProcessModeControls();
      }

      iconArrayCheckThenSet();
      sevenSegmentCheckThenSet();
      populateFixedLcdDisplay(welcome);
      minuteChange = false;
      acAmpRxChanged = false;
      acAmpMessageToProcess = false;
    }
  }
  else
  {
    acAmpOn = false;
    //This will be confMode
    confMenu();
    sevenSegmentCheckThenSet();
    iconArrayCheckThenSet();
  }
}


/*****************************************************************
// acAmp inbound
*****************************************************************/


// Need to set leds based on acAmp button state, we are directly manipulating the ports.
void acAmpLedFuntions(bool _acAmpOn)
{
  if (_acAmpOn)
  {
    PORTC = (!bitRead(acAmpRxData[4], 6)) ? PORTC |= (1 << PC5) : PORTC &= ~(1 << PC5); // FrontDemist
    PORTA = (!bitRead(acAmpRxData[4], 5)) ? PORTA |= (1 << PA4) : PORTA &= ~(1 << PA4); // RearDemist
    PORTC = (bitRead(acAmpRxData[4], 3)) ? PORTC &= ~(1 << PC7) : PORTC |= (1 << PC7); // Fresh
    PORTA = (bitRead(acAmpRxData[4], 3)) ? PORTA |= (1 << PA6) : PORTA &= ~(1 << PA6); // Recirc
    PORTC = (!bitRead(acAmpRxData[3], 6)) ? PORTC |= (1 << PC3) : PORTC &= ~(1 << PC3); // Auto
    PORTA = (!bitRead(acAmpRxData[3], 5)) ? PORTA |= (1 << PA5) : PORTA &= ~(1 << PA5); // A/C
  }
  else // When the acAmp is off, We turn off all the LED's.
  {
    PORTC &= ~(1 << PC5);
    PORTA &= ~(1 << PA4);
    PORTC &= ~(1 << PC7);
    PORTA &= ~(1 << PA6);
    PORTC &= ~(1 << PC3);
    PORTA &= ~(1 << PA5);
  }
}






/*****************************************************************
  // Icon display
  // For the icon displays we calculate a binary value for the 5 seperate registers with simple addition in hex
  //                      Position\Register 0x01,0x02,0x04,0x08,0x10
  // int icon90 = [00]; mM R/H lower, mM lower, fullstop between Minutes, Mm R/H lower, Mn lower
  // int icon91 - [01]; mM L/H upper, mM Upper, mM R/H upper, mM centre, L/H vertical divider
  // int icon92 = [02]; Mm L/H upper, Mn upper, Mm R/H upper, Mm centre, mM L/H lower
  // int icon93 = [03]; not used, not used, HH:MM Divider, Mm L/H lower, CD IN
  // int icon94 = [04]; not used, not used, not used, not used, MD IN
  // int icon95 = [05]; Dolby Logo, not used, not used, not used, ST
  // int icon96 = [06]; AF, AMB, Thermo symbol, degF, degC
  // int icon97 = [07]; Car logo (aircon), PTY, fresh air Arrow, recirculate Arrow, AUTO
  // int icon98 = [08]; fan4, RPT, fan5, A\C, ECO
  // int icon99 = [09]; not used, Fan base, fan1, fan2, fan3
  // int icon9A = [10]; Auto-M, TP, TA, RDM, Full Stop between DDRAM #10 & #11
  // int icon9B = [11]; not used, fan7, min/sec prime marks, R/H vertical divider, Full Stop between DDRAM #11 & #12
  // int icon9C = [12]; Seated Man, Front Demist, Down\Feet Arrow, not used, not used
  // int icon9D = [13]; not used, Temp Decimal, Face Arrow, fan6, mid display colon,
*****************************************************************/

void iconCalculateMode(byte _modeSet) // Mode 1 = feet, 2 = feet\demist, 3 = face, 4 = face\feet
{
  switch (_modeSet)
  {
    case 1: iconArray[0][12] += 0x04;
      break;
    case 2: if (!bitRead(acAmpRxData[4], 2) && bitRead(acAmpRxData[4], 6)) // feet/demist has dependancies on addition demist related bytes
      {
        iconArray[0][12] += 0x02;
        iconArray[0][12] += 0x04;
      }
      else if (bitRead(acAmpRxData[4], 2))
      {
        iconArray[0][12] += 0x02;
      }
      break;
    case 3: iconArray[0][13] += 0x04;
      break;
    case 4: iconArray[0][12] += 0x04; iconArray[0][13] += 0x04;
      break;
  }
}



// Set fan speed icons including the base outline
void iconCalculateFan(byte _fanSpeed)
{
  switch (_fanSpeed)
  {
    case 1: iconArray[0][9] += 0x06;
      break;
    case 2: iconArray[0][9] += 0x0E;
      break;
    case 3: iconArray[0][9] += 0x1E;
      break;
    case 4: iconArray[0][8] += 0x01; iconArray[0][9] += 0x1E;
      break;
    case 5: iconArray[0][8] += 0x05; iconArray[0][9] += 0x1E;
      break;
    case 6: iconArray[0][8] += 0x05; iconArray[0][9] += 0x1E; iconArray[0][13] += 0x08;
      break;
    case 7: iconArray[0][8] += 0x05; iconArray[0][9] += 0x1E; iconArray[0][13] += 0x08; iconArray[0][11] += 0x02;
      break;
  }
}



//Calculate the seven segment display values that are generated via the icon matrix, Mm and mM
void iconCalculateMinute(byte _digit, byte _value)
{
  if (_digit == 1) //Mm
  {
    switch (_value)
    {
      case 0: iconArray[0][0] += 0x18; iconArray[0][2] += 0x07; iconArray[0][3] += 0x08;
        break;
      case 1: iconArray[0][0] += 0x08; iconArray[0][2] += 0x04; iconArray[0][3] += 0x00;
        break;
      case 2: iconArray[0][0] += 0x10; iconArray[0][2] += 0x0E; iconArray[0][3] += 0x08;
        break;
      case 3: iconArray[0][0] += 0x18; iconArray[0][2] += 0x0E; iconArray[0][3] += 0x00;
        break;
      case 4: iconArray[0][0] += 0x08; iconArray[0][2] += 0x0D; iconArray[0][3] += 0x00;
        break;
      case 5: iconArray[0][0] += 0x18; iconArray[0][2] += 0x0B; iconArray[0][3] += 0x00;
        break;
        // we should never see these for Mm minute, but just incase someone wants them for an alternate purpose.
        // case 6: iconArray[0][0] += 0x18; iconArray[0][2] +=0x0B; iconArray[0][3] += 0x08;
        // break;
        // case 7: iconArray[0][0] += 0x08; iconArray[0][2] +=0x06; iconArray[0][3] += 0x00;
        // break;
        // case 8: iconArray[0][0] += 0x18; iconArray[0][2] +=0x0F; iconArray[0][3] += 0x08;
        // break;
        // case 9: iconArray[0][0] += 0x08; iconArray[0][2] +=0x0F; iconArray[0][3] += 0x00;
        // break;
    }
  }
  else if (_digit == 2) //mM
  {
    switch (_value)
    {
      case 0: iconArray[0][0] += 0x03; iconArray[0][1] += 0x07; iconArray[0][2] += 0x10;
        break;
      case 1: iconArray[0][0] += 0x01; iconArray[0][1] += 0x04; iconArray[0][2] += 0x00;
        break;
      case 2: iconArray[0][0] += 0x02; iconArray[0][1] += 0x0E; iconArray[0][2] += 0x10;
        break;
      case 3: iconArray[0][0] += 0x03; iconArray[0][1] += 0x0E; iconArray[0][2] += 0x00;
        break;
      case 4: iconArray[0][0] += 0x01; iconArray[0][1] += 0x0D; iconArray[0][2] += 0x00;
        break;
      case 5: iconArray[0][0] += 0x03; iconArray[0][1] += 0x0B; iconArray[0][2] += 0x00;
        break;
      case 6: iconArray[0][0] += 0x03; iconArray[0][1] += 0x0B; iconArray[0][2] += 0x10;
        break;
      case 7: iconArray[0][0] += 0x01; iconArray[0][1] += 0x06; iconArray[0][2] += 0x00;
        break;
      case 8: iconArray[0][0] += 0x03; iconArray[0][1] += 0x0F; iconArray[0][2] += 0x10;
        break;
      case 9: iconArray[0][0] += 0x03; iconArray[0][1] += 0x0F; iconArray[0][2] += 0x00;
        break;
    }
  }
}



// Default values for iconArray [0][1] vertical divider, [0][3] HH:MM divider, [0][11] vertical divider, iconArray[1][x] = hex mapping for display address.
void iconDefaultDisplayValues()
{
  iconArray[0][1] += 0x10; // Vertical divider R/H
  iconArray[0][11] += 0x08; // Vertical divider L/H
  iconArray[0][3] += 0x04; // HH:MM Divider

  if (acAmpOn)
  {
    iconArray[0][12] += 0x01; // The seated man
    iconArray[0][7] = (bitRead(acAmpRxData[4], 3)) ? iconArray[0][7] += 0x09 : iconArray[0][7] += 0x05; //airSource
    if (acAmpAmbient)
    {
      iconArray[0][6] += 0x02; // AMB
      iconArray[0][6] = (acAmpCentigrade) ? iconArray[0][6] += 0x10 : iconArray[0][6] += 0x08; // C or F
    }
    if (acAmpRunning && !acAmpAmbient)
    {
      iconArray[0][13] += 0x02; // decimal place in TMP
      iconArray[0][6] += 0x04; // Thermo symbol
    }
  }
}



void iconArrayCheckThenSet()
{
  minuteCalculate(currentMinute); //Must be recalculated every time we update Icons regardless of change.
  iconDefaultDisplayValues();     //Must be recalculated every time we update Icons regardless of change.
  for (int i = 0; i < 14; i++)
  {
    lcdDisplaySetIconDisplay(iconArray[1][i], iconArray[0][i]);
    iconArray[0][i] = 0;
  }
}


/*****************************************************************
  // SevenSegment Display
  // Calculate the seven segmeent display values that are generated via CGRam for Hh, hH, Tmp, tMp, tmP
  // Parameters; the HEX value for the bit mapped to column (0x10 = 8C Hh, 0x08 = 8C hH, 0x01 = 8D Tmp, 0x04 = 8D tMp, 0x02 = 8C tmP)
*****************************************************************/

void sevenSegmentCalculate(byte _column, byte _value)
{
  switch (_value)
  {
    case 0: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][1] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][4] += _column; sevenSegmentArray[0][5] += _column; sevenSegmentArray[0][6] += _column;
      break;
    case 1: sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][5] += _column;
      break;
    case 2: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][4] += _column; sevenSegmentArray[0][6] += _column;
      break;
    case 3: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][5] += _column; sevenSegmentArray[0][6] += _column;
      break;
    case 4: sevenSegmentArray[0][1] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][5] += _column;
      break;
    case 5: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][1] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][5] += _column; sevenSegmentArray[0][6] += _column;
      break;
    case 6: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][1] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][4] += _column; sevenSegmentArray[0][5] += _column; sevenSegmentArray[0][6] += _column;
      break;
    case 7: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][5] += _column;
      break;
    case 8: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][1] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][4] += _column; sevenSegmentArray[0][5] += _column; sevenSegmentArray[0][6] += _column;
      break;
    case 9: sevenSegmentArray[0][0] += _column; sevenSegmentArray[0][1] += _column; sevenSegmentArray[0][2] += _column; sevenSegmentArray[0][3] += _column; sevenSegmentArray[0][5] += _column;
      break;
  }
}



void sevenSegmentCheckThenSet()
{
  hourCalculate(currentHour); //we include this here as must be reclaculated everytime we update regardless of change.
  lcdDisplayPopulateCgramAddress(0x00);
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



//Process Hour as 2 separate digits
void hourCalculate(byte _currentHour)
{
  byte theHourHh = _currentHour / 10;
  sevenSegmentCalculate(0x10, theHourHh); //wirte to the sevenSegment array

  byte theHourhH = _currentHour % 10;
  sevenSegmentCalculate(0x08, theHourhH); //wirte to the sevenSegment array
}



// Process Minute as 2 separate digits
void minuteCalculate(byte _currentMinute)
{
  byte theMinuteMm = _currentMinute / 10;
  iconCalculateMinute(1, theMinuteMm); // write to the Icon array

  byte theMinutemM = _currentMinute % 10;
  iconCalculateMinute(2, theMinutemM); // write to the Icon array
}



//Define temperature control mapping - The F calculation incomplete
void acAmpProcessTemp()
{
  byte currentTemp = 0;
  byte displayOrder = 0;
  byte fTemp = 0;
  unsigned int tempOrder[3] = {0x01, 0x04, 0x02};
  for (byte i = 0; i <= 2; i++)
  {
    displayOrder = tempOrder[i];

    for (byte j = 0; j <= 3; j++)
    {
      bitWrite(currentTemp, j, bitRead(acAmpRxData[i + 1], j));
    }

    if (minuteChange || acAmpMessageToProcess) //as the temp is via seven segment, we need to make sure we update if the time changes.
    {
      sevenSegmentCalculate(displayOrder, currentTemp); //write to the sevenSegment array
    }
  }
}


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


/*****************************************************************
  // Menu
*****************************************************************/

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


/*****************************************************************
  // LCD Display
*****************************************************************/

void populateFixedLcdDisplay(String _lcdInput)
{
  _lcdInput.remove(12);

  if (_lcdInput.length() < 12)
  {
    for (byte i = _lcdInput.length() + 1; i <= 12; i++)
    {
      _lcdInput.concat(' ');
    }
  }

  byte _start = 0x80;

  PORTB &= ~(1 << PB0);
  SPI.transfer(0x80);
  PORTB |= (1 << PB0);

  for (byte i = 0; i < _lcdInput.length(); i++)
  {
    PORTL |= (1 << PL0);
    PORTB &= ~(1 << PB0);
    SPI.transfer(_lcdInput.charAt(i));
    PORTB |= (1 << PB0);
    delayMicroseconds(42);
    PORTL &= ~(1 << PL0);
  }
}



void lcdDisplaySetIconDisplay(byte _iconAddress, byte _iconValue)
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(_iconAddress);
  PORTB |= (1 << PB0);
  PORTL |= (1 << PL0);
  PORTB &= ~(1 << PB0);
  SPI.transfer(_iconValue);
  PORTB |= (1 << PB0);
  delayMicroseconds(42);
  PORTL &= ~(1 << PL0);
}



void lcdDisplayPopulateCgramAddress(byte _CgramAddress) // Set the CGRam Address
{
  PORTL |= (1 << PL1);
  PORTB &= ~(1 << PB0);
  SPI.transfer(_CgramAddress);
  PORTB |= (1 << PB0);
  PORTL &= ~(1 << PL1);
  unsigned long time01 = millis();
  for (byte i = 0; i < 7; i++) //populate from the Array
  {
    PORTL |= (1 << PL0);
    PORTB &= ~(1 << PB0);
    SPI.transfer(sevenSegmentArray[0][i]);
    PORTB |= (1 << PB0);
    delayMicroseconds(42);
    PORTL &= ~(1 << PL0);
    sevenSegmentArray[0][i] = 0;
  }
  lcdDisplayCgramToDdram(0x00, 0x8C);   // Populate into DDRam address
  lcdDisplayCgramToDdram(0x00, 0x8D);   // Populate into DDRam address
}



void  lcdDisplayCgramToDdram(byte _CgramSource, byte _DdramDestination) // Copies a CGRam char into the DDram field
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(_DdramDestination);
  PORTB |= (1 << PB0);
  PORTL |= (1 << PL0);
  PORTB &= ~(1 << PB0);
  SPI.transfer(_CgramSource);
  PORTB |= (1 << PB0);
  delayMicroseconds(42);
  PORTL &= ~(1 << PL0);
}



void lcdDisplayStartUp()
{

  PORTL &= ~(1 << PL1);
  PORTL &= ~(1 << PL0);

  //display off
  PORTL |= (1 << PL0);
  SPI.transfer(0x08);
  PORTL &= ~(1 << PL0);
  PORTB |= (1 << PB0);

  clearLcdDisplay();
  returnHomeLcdDisplay();
  flashingCursorLcdDisplay();
  setStaticPortLcdDisplay();
  entryModeIncrementLcdDisplay();
  setContrastLcdDisplay();
  IncrementLcdDisplay();
  resetIconDisplay();
  resetLcdDisplay();
}



void clearLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x1);
  PORTB |= (1 << PB0);
}



void returnHomeLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x02);
  PORTB |= (1 << PB0);
}



void flashingCursorLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x0C);
  PORTB |= (1 << PB0);
}



void setStaticPortLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x00);
  PORTB |= (1 << PB0);
}



void setContrastLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x46);
  PORTB |= (1 << PB0);
}



void entryModeIncrementLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x06);
  PORTB |= (1 << PB0);
}



void IncrementLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x14);
  PORTB |= (1 << PB0);
}



void resetIconDisplay()
{
  for (byte i = 0x90; i < 0x9F; i++)
  {
    PORTB &= ~(1 << PB0);
    SPI.transfer(i);
    PORTB |= (1 << PB0);
    PORTL |= (1 << PL0);
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x00);
    PORTB |= (1 << PB0);
    delayMicroseconds(42);
    PORTL &= ~(1 << PL0);
  }
}



void resetLcdDisplay()
{
  PORTB &= ~(1 << PB0);
  SPI.transfer(0x80);
  PORTB |= (1 << PB0);

  for (byte i = 0; i < 12; i++)
  {
    PORTL |= (1 << PL0);
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x20);
    PORTB |= (1 << PB0);
    delayMicroseconds(42);
    PORTL &= ~(1 << PL0);
  }
}





