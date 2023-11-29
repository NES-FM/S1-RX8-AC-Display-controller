#include "main.hpp"

buttonPanel buttons;
confMenu conf;
acAmp ac;
display disp;
clock time;

void setup()
{
    logger_init();
    buttons.init();
    time.init();
    ac.init();
    disp.init();
}

void loop()
{
    // Get data from AC
    // logln("Getting AC Data...");
    ac.tick();

    // Get State of Center Button Panel
    // logln("Getting Buttons state...");
    buttons.tick();
    // logln("And interpreting it...");
    shortButtonAction(buttons.lastTickButtonState.shortPushButton);
    longButtonAction(buttons.lastTickButtonState.longPushButton);
    if (conf.confMode)
        conf.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);
    else
        ac.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);

    // Get Time
    // logln("Getting time...");
    time.tick(conf.twentyFourHour, conf.confMode);

    // Set LEDs
    // logln("Setting State LEDs...");
    buttons.setLeds(ac.iconsLeds);

    // Set Display
    // logln("Setting Display...");
    if (!conf.confMode)
    {
        // logln("Not in confMode -> using ac state");
        if (ac.displayChanged || time.t.minuteChange)
        {
            // logln("AC Display changed or minuteChange");
            disp.setAcIcons(ac.iconsLeds);
            disp.setTime(time.t);

            disp.sendIcons();
            disp.sendSevenSeg();

            ac.displayChanged = false;
            time.t.minuteChange = false;
        }
    }
    else
    {
        // logln("ConfMode -> using conf state");
        conf.menuTick();
        if (conf.displayChanged || time.t.minuteChange)
        {
            // logln("Conf Display changed or minuteChange");
            disp.setAcIcons(conf.icons);
            disp.setTime(time.t);
            disp.writeToCharDisp(conf.outputText);
            disp.sendIcons();
            disp.sendSevenSeg();
        }
    }

    // Send Button data to AC
    // logln("Sending to AC...");
    if (!conf.confMode)
        ac.send();

    #ifdef debug
    // logln("End of loop...");
    // delay(100);
    #endif
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
