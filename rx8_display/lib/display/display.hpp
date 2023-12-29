#pragma once
#include <Arduino.h>
#include <SPI.h>

#include "pins.h"
#include "data_types.h"

class display {
    public:
        display() {}
        void init();
        inline void setAcIcons(acShow ic) { acIcons = ic; }
        inline void setMidIcons(midsectionIcons mi) { midIcons = mi; }
        inline void setTime(timeObj t) { time = t; }
        void sendIcons();
        void sendSevenSeg();

        /*********** 
         * AAA:AAAAAAA.'A."A
         * colon, fullstop 1, prime_marks, fullstop_2
        */
        void writeToCharDisp(String _lcdInput);

    private:
        // iconArray for display icons 90-9D, first 5 LSB, calculated via addition
        uint8_t iconArray[14] = {0};
        const uint8_t iconRegistryOffset = 0x90;
        uint8_t sevenSegmentArray[7] = {0};

        acShow acIcons;
        midsectionIcons midIcons;
        timeObj time;

        const String defaultText = "Mazda RX-8";

        void setDefaultText();
        void calculateIcons();
        void iconCalculateFan(byte _fanSpeed);
        void iconCalculateMinute(byte _digit, byte _value);
        void tempCalculate();
        void sevenSegmentCalculate(byte _column, byte _value);
        void minuteCalculate();
        void hourCalculate();
        void lcdDisplaySendIconDisplay(byte _iconAddress, byte _iconValue);
        void lcdDisplayPopulateCgramAddress(byte _CgramAddress);
        void lcdDisplayCgramToDdram(byte _CgramSource, byte _DdramDestination);
        void lcdDisplayStartUp();
        void clearLcdDisplay();
        void returnHomeLcdDisplay();
        void flashingCursorLcdDisplay();
        void setStaticPortLcdDisplay();
        void setContrastLcdDisplay();
        void entryModeIncrementLcdDisplay();
        void IncrementLcdDisplay();
        void resetIconDisplay();
        void resetLcdDisplay();
};
