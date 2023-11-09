#include "main.hpp"


/*******************
 * TODO list:
 * - Disp
 *   - icons
 *   - clock
 *   - text
 *   - temp
 * - rtc
*/


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
    // Get data from AC
    ac.tick();

    // Get State of Center Button Panel
    buttons.tick();
    shortButtonAction(buttons.lastTickButtonState.shortPushButton);
    longButtonAction(buttons.lastTickButtonState.longPushButton);
    if (conf.confMode)
        conf.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);
    else
        ac.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);

    // Get Time

    // Set LEDs
    buttons.setLeds(ac.acAmpOn, ac.iconsLeds);

    // Conf Menu

    // Set Display
    if (ac.displayChanged /*|| time.changed*/)
    {

    }

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
            default:
                break;
        }
    }
}
