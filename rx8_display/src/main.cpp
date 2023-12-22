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
backlightLedManager backlight;

void setup()
{
    logger_init();
    buttons.init();
    time.init();
    ac.init();
    disp.init();

    backlight.registerBackgroundLed(new digitalBacklightLed(footBacklight));
    backlight.registerBackgroundLed(new digitalBacklightLed(hazardBacklight));
    backlight.init();
}

void loop()
{
    // Get data from AC
    ac.tick();

    // Get State of Center Button Panel
    buttons.tick();
    if (conf.confMode)
    {
        conf.shortButtonPress(buttons.lastTickButtonState.shortPushButton);
        conf.longButtonPress(buttons.lastTickButtonState.longPushButton);
        conf.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);
        buttons.allow();
    } 
    else
    {
        ac.shortButtonPress(buttons.lastTickButtonState.shortPushButton);
        longButtonAction(buttons.lastTickButtonState.longPushButton);
        ac.changeRotary(buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation);

        // Send Button data to AC
        if (ac.send()) // If sent and not in interval
            buttons.allow();
    }

    // Get Time
    time.tick(conf.twentyFourHour, conf.confMode);

    // Serial Data
    // execute_command(logger_tick());

    // Set LEDs
    buttons.setLeds(ac.iconsLeds);

    // Set Display
    if (!conf.confMode)
    {
        if (ac.displayChanged || time.t.minuteChange)
        {
            disp.setAcIcons(ac.iconsLeds);
            disp.setTime(time.t);

            // Midsection:
            double voltage = (double)analogRead(ignitionVoltage) * 0.0146484375; // Map from 0-1024 to 0-15
            logln("Battery: %d -> %.1f", analogRead(ignitionVoltage), voltage);
            char out[16];
            sprintf(out, "BAT %.1fV", voltage);
            disp.writeToCharDisp(out, true);


            disp.sendIcons();
            disp.sendSevenSeg();

            ac.displayChanged = false;
            time.t.minuteChange = false;
        }
    }
    else
    {
        conf.menuTick();
        if (conf.displayChanged || time.t.minuteChange)
        {
            disp.setAcIcons(conf.icons);
            disp.setMidIcons(conf.midIcons);
            disp.setTime(time.t);
            disp.writeToCharDisp(conf.outputText);
            disp.sendIcons();
            disp.sendSevenSeg();
        }
    }

    // Setting backlight
    backlight.tick();
}

void longButtonAction(btn_enum longButton) {
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
