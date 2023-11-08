#pragma once
#include <Arduino.h>

#include "pins.h"
#include "data_types.h"

class buttonPanel {
    public:
        buttonPanel() {};
        void init();
        void tick();

        buttonState lastTickButtonState;

    private:
        #define BUTTON_PANEL_BUTTONS_INTERVAL 10
        unsigned long last_get_buttons = 0;

        void checkFanRotation();
        void checkTempRotation();
        void checkPushedButton();
        void checkMatrixCycle();

        // Rotary switches
        bool aFanOld = 0;
        bool bFanOld = 0;
        bool aTempOld = 0;
        bool bTempOld = 0;

        // Push button matrix stuff
        uint8_t buttonCount = 0;
        btn_enum pushedButtonOld = no_button;
        btn_enum pushedButtonCurrent = no_button;
        unsigned long buttonPushedMillis = 0;
        bool longPressButtonHeldAfterAction = false;
        const uint8_t buttonDebounce = 50;
        const uint8_t buttonLongShortThresh = 15; //Control the point where shortPushButton becomes longPushButton
};
