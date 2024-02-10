#include "buttonPanel.hpp"

void buttonPanel::init() {
    // Push buttons and rotary switches
    pinMode( matrixColA, OUTPUT );
    pinMode( matrixColB, OUTPUT );
    pinMode( matrixRow1, INPUT_PULLUP );
    pinMode( matrixRow2, INPUT_PULLUP );
    pinMode( matrixRow3, INPUT_PULLUP );
    pinMode( matrixRow4, INPUT_PULLUP );

    // Control panel LED's
    pinMode( rearDemistLED, OUTPUT );
    pinMode( frontDemistLED, OUTPUT );
    pinMode( freshAirLED, OUTPUT );
    pinMode( reCircLED, OUTPUT );
    pinMode( autoLED, OUTPUT );
    pinMode( AirConLED, OUTPUT );

    fanEncoder = new Encoder( fanInTwo, fanInOne );
    tempEncoder = new Encoder( tempInTwo, tempInOne );
}

void buttonPanel::tick() {
    if ( _allow ) {
        lastTickButtonState.fanRotation = 0;
        lastTickButtonState.tempRotation = 0;
        lastTickButtonState.shortPushButton = no_button;
        lastTickButtonState.longPushButton = no_button;
        if ( millis() - last_get_buttons >= BUTTON_PANEL_BUTTONS_INTERVAL ) {
            checkFanRotation();
            checkTempRotation();
            checkPushedButton();
            last_get_buttons = millis();

            _allow = false;
        }
    }
}

void buttonPanel::setLeds( acShow leds ) {
    if ( leds.ampRunning ) {
        digitalWrite( frontDemistLED, leds.modeFrontDemist && !leds.modeFeet && !leds.modeFace );
        digitalWrite( rearDemistLED, leds.modeRearDemist );
        digitalWrite( freshAirLED, !leds.modeRecirculate );
        digitalWrite( reCircLED, leds.modeRecirculate );
        digitalWrite( autoLED, leds.stateAuto );
        digitalWrite( AirConLED, leds.stateAc );
    } else // When the acAmp is off, We turn off all the LED's.
    {
        digitalWrite( frontDemistLED, LOW );
        digitalWrite( rearDemistLED, LOW );
        digitalWrite( freshAirLED, LOW );
        digitalWrite( reCircLED, LOW );
        digitalWrite( autoLED, LOW );
        digitalWrite( AirConLED, LOW );
    }
}

void buttonPanel::checkFanRotation() {
    lastTickButtonState.fanRotation = fanEncoder->read() / 2;
    if ( lastTickButtonState.fanRotation != 0 ) {
        fanEncoder->readAndReset();
    }
}

void buttonPanel::checkTempRotation() {
    lastTickButtonState.tempRotation = tempEncoder->read() / 2;
    if ( lastTickButtonState.tempRotation != 0 ) {
        tempEncoder->readAndReset();
    }
}

void buttonPanel::checkPushedButton() {
    checkMatrixCycle(); // Buttons have a matrix switch arrangement.

    if ( pushedButtonCurrent != pushedButtonOld ) {
        // If button released
        if ( pushedButtonCurrent == no_button ) {
            // If it was long press
            if ( longPressRegistered ) {
                longPressRegistered = false;
            } else {
                // Else it was short press
                unsigned long timedelta = millis() - buttonPushedMillis;
                // Debounce minimum time
                if ( timedelta >= 50 ) {
                    lastTickButtonState.shortPushButton = pushedButtonOld;
                }
            }
        } else {
            // Button Pushed
            buttonPushedMillis = millis();
        }
        pushedButtonOld = pushedButtonCurrent;
    } else if ( !longPressRegistered && pushedButtonCurrent != no_button ) {
        // Check if button held for long enough
        unsigned long timedelta = millis() - buttonPushedMillis;
        if ( timedelta >= 700 ) {
            lastTickButtonState.longPushButton = pushedButtonCurrent;
            longPressRegistered = true;
        }
    }
}

void buttonPanel::checkMatrixCycle() // we poll the pin matrix this takes 2 cycles and we need to set pins high\low
{
    pushedButtonCurrent = no_button;

    digitalWrite( matrixColA, HIGH );
    digitalWrite( matrixColB, LOW );
    delayMicroseconds( 2 );
    if ( !digitalRead( matrixRow1 ) )
        pushedButtonCurrent = Mode; // Mode switch
    else if ( !digitalRead( matrixRow2 ) )
        pushedButtonCurrent = frontDemist; // frontDemist switch
    else if ( !digitalRead( matrixRow3 ) )
        pushedButtonCurrent = AirSource; // AirSource switch
    else if ( !digitalRead( matrixRow4 ) )
        pushedButtonCurrent = Off; // Off switch

    digitalWrite( matrixColA, LOW );
    digitalWrite( matrixColB, HIGH );
    delayMicroseconds( 2 );
    if ( !digitalRead( matrixRow1 ) )
        pushedButtonCurrent = Auto; // Auto switch
    else if ( !digitalRead( matrixRow2 ) )
        pushedButtonCurrent = AC; // A/C switch
    else if ( !digitalRead( matrixRow3 ) )
        pushedButtonCurrent = rearDemist; // rearDemist switch
    else if ( !digitalRead( matrixRow4 ) )
        pushedButtonCurrent = _invalid; // Error if we get here with RX8
}

String buttonPanel::btn_enum_to_str( btn_enum in ) {
    switch ( in ) {
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
