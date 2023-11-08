#pragma once
#include <Arduino.h>

#include "data_types.h"

class confMenu {
    public:
        bool confMode = false;
        void shortButtonPress(btn_enum shortButton);
        void longButtonPress(btn_enum longButton);

        void activate();
        void deactivate();
    private:
};