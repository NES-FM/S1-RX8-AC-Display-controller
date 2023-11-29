#include "main.hpp"

/*********** 
 * Middle Section:
 * CD IN, MD IN, ST, Dolby Logo, AF, PTY, RPT, RDM, TA, TP, Auto-M
 * AAA:AAAAAAA.'A."A
 * mid display colon, Full Stop between DDRAM #10 & #11, min/sec prime marks, Full Stop between DDRAM #11 & #12
*/

buttonPanel buttons;
confMenu conf;
acAmp ac;
display disp;
clock time;

void setup()
{
    logger_init();
    buttons.init();
    debugln("After buttons init");
    time.init();
    debugln("After time init");
    ac.init();
    debugln("After ac init");
    disp.init();
    debugln("Endof setup");
}

void loop()
{
    // Get data from AC
    debugln("Getting AC Data...");
    ac.tick();

    // Get State of Center Button Panel
    debugln("Getting Buttons state...");
    buttons.tick();
    debugln("And interpreting it...");
    shortButtonAction(buttons.lastTickButtonState.shortPushButton);
    longButtonAction(buttons.lastTickButtonState.longPushButton);
    if (conf.confMode)
        conf.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);
    else
        ac.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);

    // Get Time
    debugln("Getting time...");
    time.tick(conf.twentyFourHour, conf.confMode);

    // Serial Data
    execute_command(logger_tick());

    // Set LEDs
    debugln("Setting State LEDs...");
    buttons.setLeds(ac.iconsLeds);

    // Set Display
    debugln("Setting Display...");
    if (!conf.confMode)
    {
        debugln("Not in confMode -> using ac state");
        if (ac.displayChanged || time.t.minuteChange)
        {
            debugln("AC Display changed or minuteChange");
            disp.setAcIcons(ac.iconsLeds);
            disp.setTime(time.t);

            disp.sendIcons();
            disp.sendSevenSeg();

            ac.displayChanged = false;
            time.t.minuteChange = false;
        }
        // TODO: Provider for midsection
    }
    else
    {
        debugln("ConfMode -> using conf state");
        conf.menuTick();
        if (conf.displayChanged || time.t.minuteChange)
        {
            debugln("Conf Display changed or minuteChange");
            disp.setAcIcons(conf.icons);
            disp.setMidIcons(conf.midIcons);
            disp.setTime(time.t);
            disp.writeToCharDisp(conf.outputText);
            disp.sendIcons();
            disp.sendSevenSeg();
        }
    }

    // Send Button data to AC
    debugln("Sending to AC...");
    if (!conf.confMode)
        ac.send();

    debugln("End of loop...");
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

void execute_command(String cmd) {
    cmd.trim();
    // logln("Parsing Command: %s", command.c_str());

    splitstring splitted = split_string_at_space(cmd);

    String top_level_command = splitted.data[0];
    String first_arg = "";
    String second_arg = "";
    String third_arg = "";

    if (splitted.length > 1)
        first_arg = splitted.data[1];
    if (splitted.length > 2)
        second_arg = splitted.data[2];
    if (splitted.length > 3)
        third_arg = splitted.data[3];

    String out;

    top_level_command.toLowerCase();
    first_arg.toLowerCase();
    second_arg.toLowerCase();
    third_arg.toLowerCase();

    if (top_level_command == "disp") {
        if (first_arg == "writestring") {
            disp.writeToCharDisp(second_arg);
        }
        else if (first_arg == "writehex") {
            String toWrite;
            toWrite = "A";
            toWrite[0] = (char)second_arg.toInt();
            disp.writeToCharDisp(toWrite);
        }
    }
    else {
        out += "Executing command " + cmd + " is currently not implemented!"; //TODO commands
    }
    logln("%s", cmd.c_str());
}
