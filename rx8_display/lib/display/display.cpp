#include "display.hpp"

void display::init()
{
    // Control pins for the display SPI
    pinMode(mode0, OUTPUT);
    pinMode(mode1, OUTPUT);
    pinMode(ssPin, OUTPUT);

    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3)); // I couldn't see a defined max speed in the NJU6623 datasheet, 10Meg is quite an increase on the original 9600, so need to monitor incase this is not stable.

    delay(300);
    lcdDisplayStartUp(); // run the LCD default settings and clear the display icons
}

void display::sendIcons()
{
    calculateIcons();
    for (int i = 0; i < 14; i++)
    {
        lcdDisplaySendIconDisplay(iconRegistryOffset + i, iconArray[i]);
        iconArray[i] = 0;
    }
}

void display::sendSevenSeg()
{
    hourCalculate();
    tempCalculate();
    lcdDisplayPopulateCgramAddress(0x00);
}

/*****************************************************************
  // Icon display
  // For the icon displays we calculate a binary value for the 5 seperate registers with simple addition in hex
  //  Position\Register 0x01,              0x02,     0x04,                     0x08,         0x10
  // int icon90 = [00]; mM R/H lower,      mM lower, fullstop between Minutes, Mm R/H lower, Mn lower
  // int icon91 - [01]; mM L/H upper,      mM Upper, mM R/H upper,             mM centre,    L/H vertical divider
  // int icon92 = [02]; Mm L/H upper,      Mn upper, Mm R/H upper,             Mm centre,    mM L/H lower
  // int icon93 = [03]; not used,          not used, HH:MM Divider,            Mm L/H lower, CD IN
  // int icon94 = [04]; not used,          not used, not used,                 not used,     MD IN
  // int icon95 = [05]; Dolby Logo,        not used, not used,                 not used,     ST
  // int icon96 = [06]; AF,                AMB,      Thermo symbol,            degF,         degC
  // int icon97 = [07]; Car logo (aircon), PTY,      fresh air Arrow,     recirculate Arrow, AUTO
  // int icon98 = [08]; fan4,              RPT,      fan5,                     A\C,          ECO
  // int icon99 = [09]; not used,          Fan base, fan1,                     fan2,         fan3
  // int icon9A = [10]; Auto-M,            TP,       TA,                       RDM,          Full Stop between DDRAM #10 & #11
  // int icon9B = [11]; not used,          fan7,   min/sec prime marks, R/H vertical divider, Full Stop between DDRAM #11 & #12
  // int icon9C = [12]; Seated Man,    Front Demist, Down\Feet Arrow,         not used,      not used
  // int icon9D = [13]; not used,      Temp Decimal, Face Arrow,              fan6,          mid display colon,
*****************************************************************/

void display::setDefaultText()
{
    writeToCharDisp(defaultText);
}

void display::calculateIcons()
{
    minuteCalculate();
    iconCalculateFan(acIcons.fanSpeed);

    // Statics
    iconArray[1] += 0x10;  // Vertical divider R/H
    iconArray[3] += 0x04;  // HH:MM Divider
    iconArray[11] += 0x08; // Vertical divider L/H

    // AC Icons
    if (acIcons.ampOn)
    {
        // The seated man
        iconArray[12] += 0x01;
        if (acIcons.modeFrontDemist)
            iconArray[12] += 0x02;
        if (acIcons.modeFeet)
            iconArray[12] += 0x04;
        if (acIcons.modeFace)
            iconArray[13] += 0x04;

        // Car Icon
        if (acIcons.modeRecirculate)
            iconArray[7] += 0x09; // Fresh air arrow + Car logo
        else
            iconArray[7] += 0x05; // Recirc arrow + Car logo

        // Ambient / Set Temperature show
        if (acIcons.displayAmbient)
        {
            iconArray[6] += 0x02; // AMB
            // TODO: if (acAmpCentigrade)
            iconArray[6] += 0x10; // degC
            // else
            //     iconArray[6] += 0x08; // degF
        }
        if (acIcons.ampRunning && !acIcons.displayAmbient)
        {
            iconArray[13] += 0x02; // decimal place in TMP
            iconArray[6] += 0x04;  // Thermo symbol
        }

        // AC States
        if (acIcons.stateAuto)
            iconArray[7] += 0x10;
        if (acIcons.stateAc)
            iconArray[8] += 0x08;
        if (acIcons.stateEco)
            iconArray[8] += 0x10;
    }

    // Midsection Icons
    if (midIcons.CD_IN)
        iconArray[3] += 0x10;
    if (midIcons.MD_IN)
        iconArray[4] += 0x10;
    if (midIcons.Dolby)
        iconArray[5] += 0x01;
    if (midIcons.ST)
        iconArray[5] += 0x10;
    if (midIcons.AF)
        iconArray[6] += 0x01;
    if (midIcons.PTY)
        iconArray[7] += 0x02;
    if (midIcons.RPT)
        iconArray[8] += 0x02;
    if (midIcons.Auto_M)
        iconArray[10] += 0x01;
    if (midIcons.TP)
        iconArray[10] += 0x02;
    if (midIcons.TA)
        iconArray[10] += 0x04;
    if (midIcons.RDM)
        iconArray[10] += 0x08;
    if (midIcons.fullstop_char_10_11)
        iconArray[10] += 0x10;
    if (midIcons.min_sec_prime_marks)
        iconArray[11] += 0x04;
    if (midIcons.fullstop_char_11_12)
        iconArray[11] += 0x10;
    if (midIcons.mid_section_colon)
        iconArray[13] += 0x10;
}

// Set fan speed icons including the base outline
void display::iconCalculateFan(byte _fanSpeed)
{
    switch (_fanSpeed)
    {
    case 1:
        iconArray[9] += 0x06;
        break;
    case 2:
        iconArray[9] += 0x0E;
        break;
    case 3:
        iconArray[9] += 0x1E;
        break;
    case 4:
        iconArray[9] += 0x1E;
        iconArray[8] += 0x01;
        break;
    case 5:
        iconArray[9] += 0x1E;
        iconArray[8] += 0x05;
        break;
    case 6:
        iconArray[9] += 0x1E;
        iconArray[8] += 0x05;
        iconArray[13] += 0x08;
        break;
    case 7:
        iconArray[9] += 0x1E;
        iconArray[8] += 0x05;
        iconArray[13] += 0x08;
        iconArray[11] += 0x02;
        break;
    }
}

// Calculate the seven segment display values that are generated via the icon matrix, Mm and mM
void display::iconCalculateMinute(byte _digit, byte _value)
{
    if (_digit == 1) // Mm
    {
        switch (_value)
        {
        case 0:
            iconArray[0] += 0x18;
            iconArray[2] += 0x07;
            iconArray[3] += 0x08;
            break;
        case 1:
            iconArray[0] += 0x08;
            iconArray[2] += 0x04;
            iconArray[3] += 0x00;
            break;
        case 2:
            iconArray[0] += 0x10;
            iconArray[2] += 0x0E;
            iconArray[3] += 0x08;
            break;
        case 3:
            iconArray[0] += 0x18;
            iconArray[2] += 0x0E;
            iconArray[3] += 0x00;
            break;
        case 4:
            iconArray[0] += 0x08;
            iconArray[2] += 0x0D;
            iconArray[3] += 0x00;
            break;
        case 5:
            iconArray[0] += 0x18;
            iconArray[2] += 0x0B;
            iconArray[3] += 0x00;
            break;
        // we should never see these for Mm minute, but just incase someone wants them for an alternate purpose.
        case 6:
            iconArray[0] += 0x18;
            iconArray[2] += 0x0B;
            iconArray[3] += 0x08;
            break;
        case 7:
            iconArray[0] += 0x08;
            iconArray[2] += 0x06;
            iconArray[3] += 0x00;
            break;
        case 8:
            iconArray[0] += 0x18;
            iconArray[2] += 0x0F;
            iconArray[3] += 0x08;
            break;
        case 9:
            iconArray[0] += 0x08;
            iconArray[2] += 0x0F;
            iconArray[3] += 0x00;
            break;
        }
    }
    else if (_digit == 2) // mM
    {
        switch (_value)
        {
        case 0:
            iconArray[0] += 0x03;
            iconArray[1] += 0x07;
            iconArray[2] += 0x10;
            break;
        case 1:
            iconArray[0] += 0x01;
            iconArray[1] += 0x04;
            iconArray[2] += 0x00;
            break;
        case 2:
            iconArray[0] += 0x02;
            iconArray[1] += 0x0E;
            iconArray[2] += 0x10;
            break;
        case 3:
            iconArray[0] += 0x03;
            iconArray[1] += 0x0E;
            iconArray[2] += 0x00;
            break;
        case 4:
            iconArray[0] += 0x01;
            iconArray[1] += 0x0D;
            iconArray[2] += 0x00;
            break;
        case 5:
            iconArray[0] += 0x03;
            iconArray[1] += 0x0B;
            iconArray[2] += 0x00;
            break;
        case 6:
            iconArray[0] += 0x03;
            iconArray[1] += 0x0B;
            iconArray[2] += 0x10;
            break;
        case 7:
            iconArray[0] += 0x01;
            iconArray[1] += 0x06;
            iconArray[2] += 0x00;
            break;
        case 8:
            iconArray[0] += 0x03;
            iconArray[1] += 0x0F;
            iconArray[2] += 0x10;
            break;
        case 9:
            iconArray[0] += 0x03;
            iconArray[1] += 0x0F;
            iconArray[2] += 0x00;
            break;
        }
    }
}

void display::tempCalculate()
{
    sevenSegmentCalculate(0x01, acIcons.tempDigits[0]);
    sevenSegmentCalculate(0x04, acIcons.tempDigits[1]);
    sevenSegmentCalculate(0x02, acIcons.tempDigits[2]);
}

// Calculate the seven segmeent display values that are generated via CGRam for Hh, hH, Tmp, tMp, tmP
// Parameters; the HEX value for the bit mapped to column (0x10 = 8C Hh, 0x08 = 8C hH, 0x01 = 8D Tmp, 0x04 = 8D tMp, 0x02 = 8C tmP)
void display::sevenSegmentCalculate(byte _column, byte _value)
{
    switch (_value)
    {
    case 0:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[1] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[4] += _column;
        sevenSegmentArray[5] += _column;
        sevenSegmentArray[6] += _column;
        break;
    case 1:
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[5] += _column;
        break;
    case 2:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[4] += _column;
        sevenSegmentArray[6] += _column;
        break;
    case 3:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[5] += _column;
        sevenSegmentArray[6] += _column;
        break;
    case 4:
        sevenSegmentArray[1] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[5] += _column;
        break;
    case 5:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[1] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[5] += _column;
        sevenSegmentArray[6] += _column;
        break;
    case 6:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[1] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[4] += _column;
        sevenSegmentArray[5] += _column;
        sevenSegmentArray[6] += _column;
        break;
    case 7:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[5] += _column;
        break;
    case 8:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[1] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[4] += _column;
        sevenSegmentArray[5] += _column;
        sevenSegmentArray[6] += _column;
        break;
    case 9:
        sevenSegmentArray[0] += _column;
        sevenSegmentArray[1] += _column;
        sevenSegmentArray[2] += _column;
        sevenSegmentArray[3] += _column;
        sevenSegmentArray[5] += _column;
        break;
    }
}

// _lcdInput is string that will be a fixed size of 12 characters
void display::writeToCharDisp(String _lcdInput)
{
    _lcdInput = _lcdInput.substring(0, 12);

    if (_lcdInput.length() < 12)
    {
        for (byte i = _lcdInput.length() + 1; i <= 12; i++)
        {
            _lcdInput.concat(' ');
        }
    }

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

// Process Minute as 2 separate digits
void display::minuteCalculate()
{
    byte theMinuteMm = time.curMinute / 10;
    iconCalculateMinute(1, theMinuteMm); // write to the Icon array

    byte theMinutemM = time.curMinute % 10;
    iconCalculateMinute(2, theMinutemM); // write to the Icon array
}
// Process Hour as 2 separate digits
void display::hourCalculate()
{
    byte theHourHh = time.curHour / 10;
    sevenSegmentCalculate(0x10, theHourHh); // wirte to the sevenSegment array

    byte theHourhH = time.curHour % 10;
    sevenSegmentCalculate(0x08, theHourhH); // wirte to the sevenSegment array
}

void display::lcdDisplaySendIconDisplay(byte _iconAddress, byte _iconValue)
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

void display::lcdDisplayPopulateCgramAddress(byte _CgramAddress) // Set the CGRam Address
{
    PORTL |= (1 << PL1);
    PORTB &= ~(1 << PB0);
    SPI.transfer(_CgramAddress);
    PORTB |= (1 << PB0);
    PORTL &= ~(1 << PL1);
    for (byte i = 0; i < 7; i++) // populate from the Array
    {
        PORTL |= (1 << PL0);
        PORTB &= ~(1 << PB0);
        SPI.transfer(sevenSegmentArray[i]);
        PORTB |= (1 << PB0);
        delayMicroseconds(42);
        PORTL &= ~(1 << PL0);
        sevenSegmentArray[i] = 0;
    }
    lcdDisplayCgramToDdram(0x00, 0x8C); // Populate into DDRam address
    lcdDisplayCgramToDdram(0x00, 0x8D); // Populate into DDRam address
}

void display::lcdDisplayCgramToDdram(byte _CgramSource, byte _DdramDestination) // Copies a CGRam char into the DDram field
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

void display::lcdDisplayStartUp()
{

    PORTL &= ~(1 << PL1);
    PORTL &= ~(1 << PL0);

    // display off
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

    delay(10);
    setDefaultText();
}

void display::clearLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x1);
    PORTB |= (1 << PB0);
}

void display::returnHomeLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x02);
    PORTB |= (1 << PB0);
}

void display::flashingCursorLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x0C);
    PORTB |= (1 << PB0);
}

void display::setStaticPortLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x00);
    PORTB |= (1 << PB0);
}

void display::setContrastLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x46);
    PORTB |= (1 << PB0);
}

void display::entryModeIncrementLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x06);
    PORTB |= (1 << PB0);
}

void display::IncrementLcdDisplay()
{
    PORTB &= ~(1 << PB0);
    SPI.transfer(0x14);
    PORTB |= (1 << PB0);
}

void display::resetIconDisplay()
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

void display::resetLcdDisplay()
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
