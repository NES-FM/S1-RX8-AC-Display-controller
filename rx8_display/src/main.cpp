#include "main.hpp"

buttonPanel buttons;
confMenu conf;
acAmp ac;

void setup()
{
    buttons.init();
    ac.init();
}

void loop()
{
    // Always initializing with default values, so that only if buttons changed different data is there
    ac.setDefault();

    // Get data from AC
    ac.tick();

    // Get State of Center Button Panel
    buttons.tick();
    // buttons.lastTickButtonState;

    // Set LEDs

    // Set Display

    // Send Button data to AC
    if (!conf.confMode)
        ac.send();
}

void shortButtonAction(btn_enum shortButton) {
    if (conf.confMode)
        conf.shortButtonPress(shortButton);
    else
        ac.shortButtonPress(shortButton);
}

void longButtonAction(btn_enum longButton) {
    if (conf.confMode)
        conf.longButtonPress(longButton);
    else
    {
        switch (longButton)
        {
            case Auto:
                ac.toggleAmbientTemp();
                break;
            case Mode:
                conf.activate();
                break;
            case AC:
                break;
            case frontDemist:
                break;
            case rearDemist:
                break;
            case AirSource:
                break;
            case Off:
                break;
        }
    }
}
