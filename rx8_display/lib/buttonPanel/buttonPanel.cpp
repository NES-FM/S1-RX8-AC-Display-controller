#include "buttonPanel.hpp"

#ifdef DEBUG_BUTTONS
#define logbtn_inline log_inline
#define logbtnln logln
#else
#define logbtn_inline
#define logbtnln
#endif

void buttonPanel::init()
{
    // Push buttons and rotary switches
    pinMode(matrixColA, OUTPUT);
    pinMode(matrixColB, OUTPUT);
    pinMode(matrixRow1, INPUT_PULLUP);
    pinMode(matrixRow2, INPUT_PULLUP);
    pinMode(matrixRow3, INPUT_PULLUP);
    pinMode(matrixRow4, INPUT_PULLUP);

    // Control panel LED's
    pinMode(rearDemistLED, OUTPUT);
    pinMode(frontDemistLED, OUTPUT);
    pinMode(freshAirLED, OUTPUT);
    pinMode(reCircLED, OUTPUT);
    pinMode(autoLED, OUTPUT);
    pinMode(AirConLED, OUTPUT);

    fanEncoder = new Encoder(fanInTwo, fanInOne);
    tempEncoder = new Encoder(tempInTwo, tempInOne);
}

void buttonPanel::tick()
{
    if (_allow)
    {
        lastTickButtonState.fanRotation = 0;
        lastTickButtonState.tempRotation = 0;
        lastTickButtonState.shortPushButton = no_button;
        lastTickButtonState.longPushButton = no_button;
        if (millis() - last_get_buttons >= BUTTON_PANEL_BUTTONS_INTERVAL)
        {
            checkFanRotation();
            checkTempRotation();
            checkPushedButton();
            last_get_buttons = millis();

            _allow = false;
        }
    }
}

void buttonPanel::setLeds(acShow leds)
{
    if (leds.ampOn)
    {
        digitalWrite(frontDemistLED, leds.modeFrontDemist);
        digitalWrite(rearDemistLED, leds.modeRearDemist);
        digitalWrite(freshAirLED, !leds.modeRecirculate);
        digitalWrite(reCircLED, leds.modeRecirculate);
        digitalWrite(autoLED, leds.stateAuto);
        digitalWrite(AirConLED, leds.stateAc);
    }
    else // When the acAmp is off, We turn off all the LED's.
    {
        digitalWrite(frontDemistLED, LOW);
        digitalWrite(rearDemistLED, LOW);
        digitalWrite(freshAirLED, LOW);
        digitalWrite(reCircLED, LOW);
        digitalWrite(autoLED, LOW);
        digitalWrite(AirConLED, LOW);
    }
}

void buttonPanel::checkFanRotation()
{
    lastTickButtonState.fanRotation = fanEncoder->read() / 2;
    if (lastTickButtonState.fanRotation != 0) {
        logbtn_inline("\nFanRotation:%d", lastTickButtonState.fanRotation);
        fanEncoder->readAndReset();
    }
}

void buttonPanel::checkTempRotation()
{
    lastTickButtonState.tempRotation = tempEncoder->read() / 2;
    if (lastTickButtonState.tempRotation != 0) {
        logbtn_inline("\nTempRotation:%d", lastTickButtonState.tempRotation);
        tempEncoder->readAndReset();
    }
}

void buttonPanel::checkPushedButton()
{
    checkMatrixCycle(); // Buttons have a matrix switch arrangement.

    if (pushedButtonCurrent != pushedButtonOld) {
        // If button released
        if (pushedButtonCurrent == no_button) {
            logbtn_inline(" Released");
            // If it was long press
            if (longPressRegistered) {
                longPressRegistered = false;
            } else {
                // Else it was short press
                unsigned long timedelta = millis() - buttonPushedMillis;
                logbtn_inline(" Short-");
                // Debounce minimum time
                if (timedelta >= 50) {
                    lastTickButtonState.shortPushButton = pushedButtonOld;
                    logbtn_inline("Press");
                }
            }
        } else {
            // Button Pushed
            buttonPushedMillis = millis();
            logbtn_inline(" Pushed at %lu", buttonPushedMillis);
        }
        pushedButtonOld = pushedButtonCurrent;
    } else if (!longPressRegistered && pushedButtonCurrent != no_button) {
        // Check if button held for long enough
        unsigned long timedelta = millis() - buttonPushedMillis;
        if (timedelta >= 700) {
            lastTickButtonState.longPushButton = pushedButtonCurrent;
            longPressRegistered = true;
            logbtn_inline(" LongPress");
        }
    }
}

void buttonPanel::checkMatrixCycle() // we poll the pin matrix this takes 2 cycles and we need to set pins high\low
{
    pushedButtonCurrent = no_button;
    // for (byte i = 0; i <= 1; i++)
    // {
    //     delayMicroseconds(2);
    //     if (i == 0)
    //     {
    //         PORTC |= (1 << PC6);
    //         PORTA &= ~(1 << PA7);
    //     }
    //     else
    //     {
    //         PORTC &= ~(1 << PC6);
    //         PORTA |= (1 << PA7);
    //     }
    //     if (!(PINA & (1 << PA2)))
    //     {
    //         if (i == 1)
    //         {
    //             pushedButtonCurrent = Auto; // Auto switch
    //             break;
    //         }
    //         else
    //         {
    //             pushedButtonCurrent = Mode; // Mode switch
    //             break;
    //         }
    //     }
    //     else if (!(PINA & (1 << PA0)))
    //     {
    //         if (i == 1)
    //         {
    //             pushedButtonCurrent = AC; // A/C switch
    //             break;
    //         }
    //         else
    //         {
    //             pushedButtonCurrent = frontDemist; // frontDemist switch
    //             break;
    //         }
    //     }
    //     else if (!(PINA & (1 << PA3)))
    //     {
    //         if (i == 1)
    //         {
    //             pushedButtonCurrent = rearDemist; // rearDemist switch
    //             break;
    //         }
    //         else
    //         {
    //             pushedButtonCurrent = AirSource; // AirSource switch
    //             break;
    //         }
    //     }
    //     else if (!(PINA & (1 << PA1)))
    //     {
    //         if (i == 1)
    //         {
    //             pushedButtonCurrent = _invalid; // Error if we get here with RX8
    //             break;
    //         }
    //         else
    //         {
    //             pushedButtonCurrent = Off; // Off switch
    //             break;
    //         }
    //     }
    // }

    digitalWrite(matrixColA, HIGH);
    digitalWrite(matrixColB, LOW);
    delayMicroseconds(2);
    if (!digitalRead(matrixRow1))
        pushedButtonCurrent = Mode; // Mode switch
    else if (!digitalRead(matrixRow2))
        pushedButtonCurrent = frontDemist; // frontDemist switch
    else if (!digitalRead(matrixRow3))
        pushedButtonCurrent = AirSource; // AirSource switch
    else if (!digitalRead(matrixRow4))
        pushedButtonCurrent = Off; // Off switch

    digitalWrite(matrixColA, LOW);
    digitalWrite(matrixColB, HIGH);
    delayMicroseconds(2);
    if (!digitalRead(matrixRow1))
        pushedButtonCurrent = Auto; // Auto switch
    else if (!digitalRead(matrixRow2))
        pushedButtonCurrent = AC; // A/C switch
    else if (!digitalRead(matrixRow3))
        pushedButtonCurrent = rearDemist; // rearDemist switch
    else if (!digitalRead(matrixRow4))
        pushedButtonCurrent = _invalid; // Error if we get here with RX8


    if (pushedButtonCurrent != no_button)
        logbtn_inline("\nPushedButtonCurrent: %s", btn_enum_to_str(pushedButtonCurrent).c_str());
}

String buttonPanel::btn_enum_to_str(btn_enum in) {
    switch (in)
    {
        case no_button:
            return "No_Button";
            break;
        case Auto:
            return "Auto";
            break;
        case Mode:
            return "Mode";
            break;
        case AC:
            return "AC";
            break;
        case frontDemist:
            return "frontDemist";
            break;
        case rearDemist:
            return "rearDemist";
            break;
        case AirSource:
            return "AirSource";
            break;
        case Off:
            return "Off";
            break;

        default:
            return "Invalid";
            break;
    }
}
