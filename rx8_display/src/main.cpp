#include "main.hpp"


/*******************
 * TODO list:
 * - rtc
 * - conf menu
*/


buttonPanel buttons;
confMenu conf;
acAmp ac;
display disp;

void setup()
{
    buttons.init();
    ac.init();
    disp.init();
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
    buttons.setLeds(ac.iconsLeds);

    // Set Display
    if (!conf.confMode)
    {
        if (ac.displayChanged /*|| time.changed*/)
        {
            disp.setAcIcons(ac.iconsLeds);
            //disp.setTime(...);

            disp.sendIcons();
            disp.sendSevenSeg();

            ac.displayChanged = false;
            // time.changed = false;
        }
    }
    else
    {
        //TODO Conf mode
        /*
        conf.menuTick();
        if (conf.displayChanged || time.changed)
        {
            disp.setAcIcons(conf.icons);
            disp.setTime(time)
            disp.sendIcons();
            disp.sendSevenSeg();
        }
        */
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
