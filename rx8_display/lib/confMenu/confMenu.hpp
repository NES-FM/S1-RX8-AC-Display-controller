#pragma once
#include <Arduino.h>

#include "data_types.h"

class confMenu {
    public:
        void activate();
        void deactivate();

        void menuTick();

        void shortButtonPress(btn_enum shortButton);
        void longButtonPress(btn_enum longButton);
        void changeRotary(rotationDir fan, rotationDir temp);

        bool confMode = false;
        bool twentyFourHour = true;
        bool displayChanged = false;
        acShow icons;
        String outputText;
    private:
};
