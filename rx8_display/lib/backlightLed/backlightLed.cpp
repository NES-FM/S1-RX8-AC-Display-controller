#include "backlightLed.hpp"

backlightLedManager::backlightLedManager() {}

void backlightLedManager::registerBackgroundLed( backlightLedBase *item ) {
    if ( num_el_in_bg_vector < bg_vector_size ) {
        bg_vector[ num_el_in_bg_vector ] = item;
        num_el_in_bg_vector++;
    } else {
        logln( "ERROR!!! BG_VECTOR IS NOT BIG ENOUGH" );
        delay( 5000 );
    }
}

void backlightLedManager::init() {
    for ( int i = 0; i < num_el_in_bg_vector; i++ ) {
        bg_vector[ i ]->init();
    }

    pinMode( backlightNegative, INPUT );
    pinMode( backlightPositive, INPUT );
}

void backlightLedManager::tick() {
    if ( millis() - lastBacklightMillis > 100 ) {
        lastBacklightMillis = millis();
        bool on = analogRead( backlightPositive ) > 512;
        int an = analogRead( backlightNegative );
        uint16_t brightness = constrain( map( an, 100, 800, UINT16_MAX, 0 ), 0, UINT16_MAX );
        // Serial.print(an); Serial.print("\t"); Serial.println(brightness);
        for ( int i = 0; i < num_el_in_bg_vector; i++ ) {
            bg_vector[ i ]->setValue( on, brightness );
        }
    }
}

digitalBacklightLed::digitalBacklightLed( int pin ) : backlightLedBase() { _pin = pin; }

void digitalBacklightLed::init() { pinMode( _pin, OUTPUT ); }

void digitalBacklightLed::setValue( bool on, uint16_t brightness ) { digitalWrite( _pin, on ); }

pwmBacklightLed::pwmBacklightLed( int pin ) {
    if ( 0 <= pin && pin <= 13 ) {
        _pin = pin;
        pinMode( _pin, OUTPUT );
        enabled = true;
    } else {
        logln( "Output disabled. PWM Pin %d is not in valid range of 0-13!", pin );
        enabled = false;
    }
}

void pwmBacklightLed::init() { pinMode( _pin, OUTPUT ); }

void pwmBacklightLed::setValue( bool on, uint16_t brightness ) {
    if ( enabled ) {
        if ( on )
            analogWrite( _pin, map( brightness, 0, UINT16_MAX, 0, 255 ) );
        else
            digitalWrite( _pin, LOW );
    }
}
