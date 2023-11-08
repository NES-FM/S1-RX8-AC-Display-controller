#include "buttonPanel.hpp"

void buttonPanel::init()
{
    // Push buttons and rotary switches
    pinMode(matrixColA, OUTPUT);
    pinMode(matrixColB, OUTPUT);
    pinMode(matrixRow1, INPUT_PULLUP);
    pinMode(matrixRow2, INPUT_PULLUP);
    pinMode(matrixRow3, INPUT_PULLUP);
    pinMode(matrixRow4, INPUT_PULLUP);
    pinMode(fanInOne, INPUT_PULLUP);
    pinMode(fanInTwo, INPUT_PULLUP);
    pinMode(tempInOne, INPUT_PULLUP);
    pinMode(tempInTwo, INPUT_PULLUP);
}

void buttonPanel::tick()
{
    lastTickButtonState.fanRotation = none;
    lastTickButtonState.tempRotation = none;
    lastTickButtonState.shortPushButton = no_button;
    lastTickButtonState.longPushButton = no_button;

    if (millis() - last_get_buttons >= BUTTON_PANEL_BUTTONS_INTERVAL)
    {
        checkFanRotation();
        checkTempRotation();
        checkPushedButton();
        last_get_buttons = millis();
    }
}

void buttonPanel::checkFanRotation()
{
    bool aFanNew = 0;
    bool bFanNew = 0;
    aFanNew = (PINC & (1 << PC4));
    bFanNew = (PINC & (1 << PC1));

    if (aFanNew != aFanOld || bFanNew != bFanOld)
    {
        if ((aFanNew && !aFanOld && !bFanNew && !bFanOld) ||
            (!aFanNew && aFanOld && bFanNew && bFanOld)) // Right movement
        {
            lastTickButtonState.fanRotation = right;
        }
        else if ((aFanNew && aFanOld && !bFanNew && bFanOld) ||
                 (!aFanNew && !aFanOld && bFanNew && !bFanOld)) // Left movement
        {
            lastTickButtonState.fanRotation = left;
        }
    }
    aFanOld = aFanNew;
    bFanOld = bFanNew;
}

void buttonPanel::checkTempRotation()
{
    bool aTempNew = 0;
    bool bTempNew = 0;
    aTempNew = (PINC & (1 << PC0));
    bTempNew = (PINC & (1 << PC2));

    if (aTempNew != aTempOld || bTempNew != bTempOld)
    {
        if ((aTempNew && !aTempOld && !bTempNew && !bTempOld) || (!aTempNew && aTempOld && bTempNew && bTempOld)) // Right movement
        {
            lastTickButtonState.tempRotation = right;
        }
        else if ((aTempNew && aTempOld && !bTempNew && bTempOld) || (!aTempNew && !aTempOld && bTempNew && !bTempOld)) // Left movement
        {
            lastTickButtonState.tempRotation = left;
        }
    }
    aTempOld = aTempNew;
    bTempOld = bTempNew;
}

void buttonPanel::checkPushedButton()
{
    checkMatrixCycle(); // Buttons have a matrix switch arrangement.

    if (pushedButtonCurrent == no_button && longPressButtonHeldAfterAction) // Long button press is single shot, so must be reset by removing finger.
    {
        longPressButtonHeldAfterAction = false;
    }

    // if (any button pressed) && (only in some interval) && (if its the same as last time, only if not already long press)
    if (pushedButtonCurrent != no_button && (millis() - buttonPushedMillis >= buttonDebounce) && (!longPressButtonHeldAfterAction && pushedButtonCurrent == pushedButtonOld)) // index the button count
    {
        buttonCount++;
        buttonPushedMillis = millis();
    }

    // If button released
    if (pushedButtonCurrent != pushedButtonOld && pushedButtonCurrent == 0)
    {
        if (buttonCount < buttonLongShortThresh)
        {
            lastTickButtonState.shortPushButton = pushedButtonOld;
            buttonPushedMillis = millis();
            pushedButtonOld = pushedButtonCurrent;
            buttonCount = 0;
        }
    }

    // Button Held for long enough
    if (!longPressButtonHeldAfterAction && pushedButtonCurrent && buttonCount > buttonLongShortThresh) // Do something with a long button press
    {
        lastTickButtonState.longPushButton = pushedButtonOld;
        longPressButtonHeldAfterAction = true;
    }
}

void buttonPanel::checkMatrixCycle() // we poll the pin matrix this takes 2 cycles and we need to set pins high\low
{
    pushedButtonCurrent = no_button;
    for (byte i = 0; i <= 1; i++)
    {
        delayMicroseconds(2);
        if (i == 0)
        {
            PORTC |= (1 << PC6);
            PORTA &= ~(1 << PA7);
        }
        else
        {
            PORTC &= ~(1 << PC6);
            PORTA |= (1 << PA7);
        }

        if (!(PINA & (1 << PA2)))
        {
            if (i == 1)
            {
                pushedButtonCurrent = Auto; // Auto switch
                break;
            }
            else
            {
                pushedButtonCurrent = Mode; // Mode switch
                break;
            }
        }
        else if (!(PINA & (1 << PA0)))
        {
            if (i == 1)
            {
                pushedButtonCurrent = AC; // A/C switch
                break;
            }
            else
            {
                pushedButtonCurrent = frontDemist; // frontDemist switch
                break;
            }
        }
        else if (!(PINA & (1 << PA3)))
        {
            if (i == 1)
            {
                pushedButtonCurrent = rearDemist; // rearDemist switch
                break;
            }
            else
            {
                pushedButtonCurrent = AirSource; // AirSource switch
                break;
            }
        }
        else if (!(PINA & (1 << PA1)))
        {
            if (i == 1)
            {
                pushedButtonCurrent = _invalid; // Error if we get here with RX8
                break;
            }
            else
            {
                pushedButtonCurrent = Off; // Off switch
                break;
            }
        }
    }
}