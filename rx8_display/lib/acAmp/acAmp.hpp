#pragma once
#include <Arduino.h>

#include "pins.h"
#include "data_types.h"
#include "logger.h"

class acAmp {
    public:
        acAmp() {}
        void init();
        void tick();
        void send();
        void setDefault();
        void shortButtonPress(btn_enum shortButton);
        void changeRotary(int fan, int temp);
        void toggleAmbientTemp();

        acShow iconsLeds;

        bool displayChanged = false;

    private:
        //acAmpTX
        uint8_t txData[5];
        const uint8_t txDefault[5] = {0x04, 0x80, 0x80, 0x80, 0xFC}; //Default data for the A\C amplifier TX; basicly it means I am here and no buttons have been pressed.
        unsigned long lastTx;
        const unsigned long txInterval = 18; //Control the cycle time otherwise we have a continuous data stream allowing for processing time this equates to 6-7 ms (most of the time).

        //RX data stuff
        uint8_t curRxByte = 0;
        unsigned int receivedByte;
        unsigned int rxData[6]; //Where we keep our inbound data.
        unsigned long lastRx;
        bool rxChanged = true;
        bool messageToProcess = false;

        bool ambientTemp = false;

        void interpretStatusByte();
        void interpretModeSettings();
        void interpretFanSpeed();
        void interpretAcState();
        void interpretTemp();
        void getSerialData();
        void acAmpSend();
};
