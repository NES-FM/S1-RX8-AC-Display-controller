#include "main.hpp"

buttonPanel buttons;
confMenu conf;
acAmp ac;
display disp;
clock time;
backlightLedManager backlight;
espComm esp( Serial3 );

midsectionIcons midIcons;
menu mainMenu;

Smoothed<double> bat_volt;

float motor_temp = 100;

void setup() {
    logger_init();
    buttons.init();
    time.init();
    ac.init();
    disp.init();
    esp.init();

    backlight.registerBackgroundLed( new digitalBacklightLed( footBacklight ) );
    backlight.registerBackgroundLed( new pwmBacklightLed( hazardBacklight ) );
    backlight.init();

    mainMenu.registerPage( new menuPage() ); // Set Page 0 to a dummy page
    mainMenu.registerPage( new value_func_page( "BAT", getBatVolt, "V", 1 ) );
    mainMenu.registerPage( new value_ptr_page( "TMP", &motor_temp, "C", 1 ) );

    bat_volt.begin( SMOOTHED_AVERAGE, 10 );
}

void loop() {
    // Get data from AC
    ac.tick();

    // Get State of Center Button Panel
    buttons.tick();
    if ( conf.confMode ) {
        conf.shortButtonPress( buttons.lastTickButtonState.shortPushButton );
        conf.longButtonPress( buttons.lastTickButtonState.longPushButton );
        conf.changeRotary( buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation );
        buttons.allow();
    } else {
        ac.shortButtonPress( buttons.lastTickButtonState.shortPushButton );
        longButtonAction( buttons.lastTickButtonState.longPushButton );
        ac.changeRotary( buttons.lastTickButtonState.fanRotation, buttons.lastTickButtonState.tempRotation );

        // Send Button data to AC
        if ( ac.send() ) // If sent and not in interval
            buttons.allow();
    }

    // Get Time
    time.tick( conf.twentyFourHour, conf.confMode );

    // Serial Data
    // execute_command(logger_tick());

    // Set LEDs
    buttons.setLeds( ac.iconsLeds );

    // Set Display
    if ( !conf.confMode ) {
        if ( ac.displayChanged || time.t.minuteChange ) {
            disp.setAcIcons( ac.iconsLeds );
            disp.setTime( time.t );

            midsectionHandler();
            disp.setMidIcons( midIcons );

            disp.sendIcons();
            disp.sendSevenSeg();

            ac.displayChanged = false;
            time.t.minuteChange = false;
        }
    } else {
        conf.menuTick();
        if ( conf.displayChanged || time.t.minuteChange ) {
            disp.setAcIcons( conf.icons );
            disp.setMidIcons( conf.midIcons );
            disp.setTime( time.t );

            disp.writeToCharDisp( conf.outputText );

            disp.sendIcons();
            disp.sendSevenSeg();

            conf.displayChanged = false;
            time.t.minuteChange = false;
        }
    }

    // Setting backlight
    backlight.tick();

    esp.tick();
}

void longButtonAction( btn_enum longButton ) {
    switch ( longButton ) {
    case Auto:
        ac.toggleAmbientTemp();
        break;
    case Mode:
        conf.activate();
        break;
    case AC:
        mainMenu.next();
        break;
    case frontDemist:
        mainMenu.previous();
        break;
    case rearDemist:
        mainMenu.next();
        break;
    case AirSource:
        mainMenu.previous();
        break;
    case Off:
        break;
    default:
        break;
    }
    buttons.lastTickButtonState.longPushButton = no_button;
}

unsigned long lastMidsectionMillis = 0;

void midsectionHandler() {
    if ( millis() - lastMidsectionMillis > 300 ) {
        midIcons.mid_section_colon = true;

        bat_volt.add( (double)analogRead( ignitionVoltage ) *
                      0.01487643158529234 ); // Map from 0-1024 to 0-15   :    12.34 real => 12.0-12.3 measured

        if ( mainMenu.cur_page_num() == 0 ) {
            midIcons.Dolby = false;

            if ( getBatVolt() > 13.5 ) { // Motor Running
                motor_temp = 100;        // TODO: Get motor temperature via canbus

                if ( motor_temp <= 90 ) { // Motor Cold
                    disp.writeToCharDisp( mainMenu.drawPage( 2 ) );
                } else { // Motor Hot
                    midIcons.mid_section_colon = false;
                    disp.writeToCharDisp( "Mazda RX-8" ); // TODO: Calculate fuel consumption
                }
            } else { // Motor Off
                disp.writeToCharDisp( mainMenu.drawPage( 1 ) );
            }
        } else {
            midIcons.Dolby = true;
            disp.writeToCharDisp( mainMenu.draw() );
        }

        lastMidsectionMillis = millis();
    }
}

void execute_command( String cmd ) {
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
}

float getBatVolt() { return bat_volt.get(); }
