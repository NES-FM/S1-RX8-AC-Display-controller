#include "main.hpp"

buttonPanel buttons;
acAmp ac;
display disp;
clock time;
backlightLedManager backlight;
espComm esp( Serial3 );
dsp sub_rear_dsp( dspWriteProtect );

midsectionIcons midIcons;

mainMenu_c mainMenu;
confMenu conf;
subVolMenu sub;
baseMenu* activeMenu;

Smoothed<double> bat_volt;

float motor_temp = 100;

void setup() {
    logger_init();
    Wire.begin();

    buttons.init();
    time.init();
    ac.init();
    disp.init();
    esp.init();
    sub_rear_dsp.init();

    backlight.registerBackgroundLed( new digitalBacklightLed( footBacklight ) );
    backlight.registerBackgroundLed( new digitalBacklightLed( hazardBacklight ) );
    backlight.init();

    mainMenu.registerPage( new mainMenuPage() ); // Set Page 0 to a dummy page
    mainMenu.bat_page = new mainMenuFuncPage( "BAT", getBatVolt, "V", 1 );
    mainMenu.registerPage( mainMenu.bat_page );
    mainMenu.temp_page = new mainMenuPtrPage( "TMP", &motor_temp, "C", 1 );
    mainMenu.registerPage( mainMenu.temp_page );

    activeMenu = &mainMenu;

    sub._dsp = &sub_rear_dsp;

    bat_volt.begin( SMOOTHED_AVERAGE, 10 );
}

void loop() {
    // Get data from AC
    ac.tick();

    // Get State of Center Button Panel
    buttons.tick();
    longButtonAction( buttons.lastTickButtonState.longPushButton );

    if (mainMenu.isActive()) {
        ac.shortButtonPress( buttons.lastTickButtonState.shortPushButton );
        ac.changeRotary( buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation );

        // Send Button data to AC
        if ( ac.send() ) // If sent and not in interval
            buttons.allow();
    } else {
        activeMenu->shortButtonPress( buttons.lastTickButtonState.shortPushButton );
        activeMenu->changeRotary( buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation );
        buttons.allow();
    }

    // Get Time
    time.tick( conf.twentyFourHour, conf.isActive() );

    // Serial Data
    // execute_command(logger_tick());

    // Set LEDs
    buttons.setLeds( ac.iconsLeds );

    // Set Display
    if ( time.t.minuteChange || activeMenu->displayChanged() || (mainMenu.isActive() && ac.displayChanged)) {
        if (mainMenu.isActive()) disp.setAcIcons( ac.iconsLeds );
        else disp.setAcIcons( activeMenu->icons );

        disp.setTime( time.t );

        disp.writeToCharDisp( activeMenu->draw() );
        disp.setMidIcons( activeMenu->midIcons );

        disp.sendIcons();
        disp.sendSevenSeg();

        ac.displayChanged = false;
        time.t.minuteChange = false;
    }

    // Others
    backlight.tick();
    esp.tick();
    sub_rear_dsp.tick();
}

void longButtonAction( btn_enum longButton ) {
    if (longButton != btn_enum::no_button) logln("longButtonAction %s", buttonPanel::btn_enum_to_str(longButton).c_str());
    switch ( longButton ) {
    case Auto:
        ac.toggleAmbientTemp();
        break;
    case Mode:
        toggleMenu(&conf);
        break;
    case AC:
        activeMenu->next();
        break;
    case frontDemist:
        activeMenu->previous();
        break;
    case rearDemist:
        activeMenu->next();
        break;
    case AirSource:
        activeMenu->previous();
        break;
    case Off:
        toggleMenu(&sub);
        break;
    default:
        break;
    }
    buttons.lastTickButtonState.longPushButton = no_button;
}

void toggleMenu( baseMenu *newMenu ) {
    
    if (newMenu->isActive()) {
        newMenu->deactivate();
        activeMenu = &mainMenu;
    } else {
        activeMenu->deactivate();
        activeMenu = newMenu;
    }
    
    activeMenu->activate();
}

/*void execute_command( String cmd ) {
    cmd.trim();
    // logln("Parsing Command: %s", command.c_str());

    splitstring splitted = split_string_at_space( cmd );

    String top_level_command = splitted.data[0];
    String first_arg = "";
    String second_arg = "";
    String third_arg = "";

    if ( splitted.length > 1 )
        first_arg = splitted.data[1];
    if ( splitted.length > 2 )
        second_arg = splitted.data[2];
    if ( splitted.length > 3 )
        third_arg = splitted.data[3];

    String out;

    top_level_command.toLowerCase();
    first_arg.toLowerCase();
    second_arg.toLowerCase();
    third_arg.toLowerCase();

    if ( top_level_command == "disp" ) {
        if ( first_arg == "writestring" ) {
            disp.writeToCharDisp( second_arg );
        } else if ( first_arg == "writehex" ) {
            String toWrite;
            toWrite = "A";
            toWrite[0] = (char)second_arg.toInt();
            disp.writeToCharDisp( toWrite );
        }
    } else {
        out += "Executing command " + cmd + " is currently not implemented!"; // TODO commands
    }
    logln( "%s", cmd.c_str() );
}*/

float getBatVolt() { 
    bat_volt.add( (double)analogRead( ignitionVoltage ) *
                      0.01487643158529234 ); // Map from 0-1024 to 0-15   :    12.34 real => 12.0-12.3 measured
    return bat_volt.get(); 
}
