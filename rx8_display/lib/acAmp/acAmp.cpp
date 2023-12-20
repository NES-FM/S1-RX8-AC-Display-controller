#include "acAmp.hpp"

void acAmp::init()
{
    Serial2.begin(4800, SERIAL_8E1);
    setDefault();
}

void acAmp::tick()
{
    // debugln("Getting Serial Data");
    getSerialData();
    if (messageToProcess)
    {
        // debugln("AC Received!");
        if (iconsLeds.ampOn)
        {
            // debugln("ampOn");
            interpretStatusByte();
            interpretAcState();
            interpretFanSpeed();
            interpretModeSettings();
            interpretTemp();
        }

        displayChanged = true;
        rxChanged = false;
        messageToProcess = false;
    }
}

void acAmp::send()
{
    if ((millis() - lastTx >= txInterval))
    {
        acAmpSend();
    }
}

void acAmp::setDefault()
{
    memcpy(txData, txDefault, 5);
}

void acAmp::shortButtonPress(btn_enum shortButton)
{
    // Reset some bytes to default values
    txData[2] = 0x80;
    txData[4] = 0xFC;

    switch (shortButton)
    {
    case Auto:
        txData[1] = 0x82;
        txData[4] = 0xFA;
        break;
    case Mode:
        txData[1] = 0x90;
        txData[4] = 0xEC;
        break;
    case AC:
        txData[1] = 0x84;
        txData[4] = 0xF8;
        break;
    case frontDemist:
        txData[1] = 0xA0;
        txData[4] = 0xDC;
        break;
    case rearDemist:
        txData[1] = 0xC0;
        txData[4] = 0xBC;
        break;
    case AirSource:
        txData[1] = 0x88;
        txData[4] = 0xF4;
        break;
    case Off:
        txData[1] = 0x81;
        txData[4] = 0xFB;
        break;
    default:
        txData[1] = 0x80;
        break;
    }
}

void acAmp::changeRotary(int fan, int temp)
{
    txData[3] = 0x80;
    if (fan > 0) {
        txData[3] = 0x90;
        txData[4] = 0xEC;
    }
    else if (fan < 0) {
        txData[3] = 0xF0;
        txData[4] = 0x8C;
    }
    if (temp > 0) {
        txData[3] = 0x81;
        txData[4] = 0xFB;
    }
    else if (temp < 0) {
        txData[3] = 0x87;
        txData[4] = 0xF5;
    }
}

void acAmp::toggleAmbientTemp()
{
    ambientTemp = !ambientTemp;
    txData[2] = 0xA0;
    txData[4] = 0xDC;
    iconsLeds.displayAmbient = ambientTemp;
}

void acAmp::interpretStatusByte() // Based on the first byte of our inbound message we have 3 main data states from AC Amplifier. Off, Normal control and Ambient
{
    if (rxData[0] == 0x0F) // we are off apart from showing Mode settings
    {
        iconsLeds.ampRunning = false;
    }

    ambientTemp = (rxData[0] == 0x0E) ? true : false; // 0D Normal || 0E Ambient
    iconsLeds.displayAmbient = ambientTemp;
}

void acAmp::interpretModeSettings()
{
    if (bitRead(rxData[4], 0) && bitRead(rxData[4], 1))
    {
        iconsLeds.modeFrontDemist = false;
        iconsLeds.modeFeet = true;
        iconsLeds.modeFace = false;
    }
    else if (bitRead(rxData[4], 0) && !bitRead(rxData[4], 1))
    {
        if (!bitRead(rxData[4], 2) && bitRead(rxData[4], 6)) // if front demist but not general demist
        {
            iconsLeds.modeFrontDemist = true;
            iconsLeds.modeFeet = true;
            iconsLeds.modeFace = false;
        }
        else if (bitRead(rxData[4], 2)) // If general demist
        {
            iconsLeds.modeFrontDemist = true;
            iconsLeds.modeFeet = false;
            iconsLeds.modeFace = false;
        }
    }
    else if (!bitRead(rxData[4], 0) && !bitRead(rxData[4], 1))
    {
        iconsLeds.modeFrontDemist = false;
        iconsLeds.modeFeet = false;
        iconsLeds.modeFace = true;
    }
    else if (!bitRead(rxData[4], 0) && bitRead(rxData[4], 1))
    {
        iconsLeds.modeFrontDemist = false;
        iconsLeds.modeFeet = true;
        iconsLeds.modeFace = true;
    }

    iconsLeds.modeRearDemist = !bitRead(rxData[4], 5);
    iconsLeds.modeRecirculate = bitRead(rxData[4], 3);
}

void acAmp::interpretFanSpeed() // Find the current fan speed
{
    byte acAmpFanSpeed = 0;
    bitWrite(acAmpFanSpeed, 2, bitRead(rxData[2], 6));
    bitWrite(acAmpFanSpeed, 1, bitRead(rxData[2], 5));
    bitWrite(acAmpFanSpeed, 0, bitRead(rxData[2], 4));

    // debugln("Fan Speed: %d = %s%s%s", acAmpFanSpeed, bitRead(rxData[2], 6), bitRead(rxData[2], 5), bitRead(rxData[2], 4));

    if (acAmpFanSpeed <= 6) // See Wiki
    {
        acAmpFanSpeed++;
        iconsLeds.ampRunning = true;
    }
    else
    {
        acAmpFanSpeed = 0;
        iconsLeds.ampRunning = false;
    }

    iconsLeds.fanSpeed = acAmpFanSpeed;
}

void acAmp::interpretAcState() // The 3 A/C States
{
    iconsLeds.stateEco =  !bitRead(rxData[3], 4);
    iconsLeds.stateAc =   !bitRead(rxData[3], 5);
    iconsLeds.stateAuto = !bitRead(rxData[3], 6);
    // debugln("AC State: %s%s%s", iconsLeds.stateEco ? "Eco" : "", iconsLeds.stateAc ? "Ac" : "", iconsLeds.stateAuto ? "Auto" : "");
}

// Define temperature control mapping
void acAmp::interpretTemp()
{
    byte currentTemp = 0;
    for (byte i = 0; i <= 2; i++)
    {
        for (byte j = 0; j <= 3; j++)
        {
            bitWrite(currentTemp, j, bitRead(rxData[i + 1], j));
        }

        iconsLeds.tempDigits[i] = currentTemp;
    }
}

// Get data from the acAmplifier. Note we don't sit and wait around for a full payload.
void acAmp::getSerialData()
{
    // log_inline_begin(); log_inline("AC received byte ");
    if (Serial2.available() > 0)
    { 
        iconsLeds.ampOn = true;
        lastRx = millis();
        receivedByte = Serial2.read();
        if (receivedByte == 0x0F || receivedByte == 0x0D || receivedByte == 0x0E) // We don't know the last byte but we know the first byte will be one of these
        {
            curRxByte = 0;
            if (receivedByte != rxData[curRxByte])
            {
                rxData[curRxByte] = receivedByte;
                rxChanged = true;
            }
            // log_inline(" %d:%0x", curRxByte, receivedByte);
        }
        else
        {
            curRxByte++;
            // log_inline("as byte %d ", curRxByte);
            if (receivedByte != rxData[curRxByte])
            {
                rxData[curRxByte] = receivedByte;
                rxChanged = true;
                // log_inline("marked as changed ");
            }
            // log_inline(" %d:%0x", curRxByte, receivedByte);
            if (curRxByte == 5) // we know the length of the data transfer. If we knew how to calculate the checksum we could validate received data but for now we assume it's correct.
            {
                // log_inline("and last byte ");
                // iconsLeds.ampOn = rxData[4] != 0xFF; // Stops icon flicker at startup by pretending the ac is off until everything is settled
                messageToProcess = true;
                curRxByte = 0;
                // log_inline("!");
                // log_inline_end();
                // TODO: Need to clear the input buffer, if arduino starts lagging behind
            }
        }
    }
    else if (millis() - lastRx > 30) // If there is no data we assume the A\C amplifier is off and we run 1 cycle to clear the display. This is easier than having to monitor assorted vehicle power states.
    {
        // log_inline("  --  Would have set amp as off because of too long interval --  ");
        // iconsLeds.ampOn = false;
        // messageToProcess = true;
        // rxChanged = true;
    }
    // log_inline_end();
}

void acAmp::acAmpSend() // Send hardware changes to the AC Amplifier
{
    for (byte acAmpCycle = 0; acAmpCycle < 5; acAmpCycle++)
    {
        Serial2.write(txData[acAmpCycle]);
    }
    lastTx = millis();
}
