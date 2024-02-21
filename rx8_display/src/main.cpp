#include "main.hpp"

buttonPanel buttons;
confMenu conf;
acAmp ac;
display disp;
clock time;
backlightLedManager backlight;
espComm esp( Serial3 );

midsectionIcons midIcons;

Smoothed<double> bat_volt;

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

unsigned long lastMidsectionMillis = 0;
char ch = 0;

void midsectionHandler() {
    if ( millis() - lastMidsectionMillis > 300 ) {
        midIcons.mid_section_colon = false;

        bat_volt.add( (double)analogRead( ignitionVoltage ) *
                      0.01487643158529234 ); // Map from 0-1024 to 0-15   :    12.34 real => 12.0-12.3 measured
        double voltage = bat_volt.get();

        if ( voltage > 13.5 ) {     // Motor Running
            double motorTemp = 100; // TODO: Get motor temperature via canbus

            if ( motorTemp <= 90 ) { // Motor Cold
                char tempStr[6];
                dtostrf( motorTemp, 4, 1, tempStr );
                disp.writeToCharDisp( "TMP " + String( tempStr ) + "°C" );
                midIcons.mid_section_colon = true;
            } else {                                  // Motor Hot
                disp.writeToCharDisp( "Mazda RX-8" ); // TODO: Calculate fuel consumption by getting values via canbus
            }
        } else { // Motor Off
            char voltageStr[6];
            dtostrf( voltage, 4, 1, voltageStr );
            disp.writeToCharDisp( "BAT " + String( voltageStr ) + "V" );
            midIcons.mid_section_colon = true;
        }
        lastMidsectionMillis = millis();
    }

    // if (millis() - lastMidsectionMillis > 1000) {
    //     lastMidsectionMillis = millis();

    //     midIcons.mid_section_colon = true;
    //     disp.writeToCharDisp(String((int)ch) + " " + ch);
    //     ch++;
    // }
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
