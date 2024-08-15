#pragma once
#include <Arduino.h>
#include <Encoder.h>

#include "data_types.h"
#include "logger.h"
#include "pins.h"

class buttonPanel {
  public:
    buttonPanel(){};
    void init();
    void tick();
    void allow() { _allow = true; }
    void setLeds( acShow leds );

    buttonState lastTickButtonState;

    static String btn_enum_to_str( btn_enum in );

  private:
#define BUTTON_PANEL_BUTTONS_INTERVAL 10
    unsigned long last_get_buttons = 0;

    void checkFanRotation();
    void checkTempRotation();
    void checkPushedButton();
    void checkMatrixCycle();

    bool _allow = true;

    // Rotary switches
    bool aFanOld = 0;
    bool bFanOld = 0;
    bool aTempOld = 0;
    bool bTempOld = 0;

    // Push button matrix stuff
    btn_enum pushedButtonOld = no_button;
    btn_enum pushedButtonCurrent = no_button;
    unsigned long buttonPushedMillis = 0;
    bool longPressRegistered = false;

    Encoder *fanEncoder;
    Encoder *tempEncoder;
};
