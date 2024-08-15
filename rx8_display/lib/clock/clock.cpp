#include "clock.hpp"

void clock::init() {
    Wire.beginTransmission( DS3231_I2C_ADDRESS );
    uint8_t error = Wire.endTransmission();

    if ( error == 0 ) {
        debugln( "RTC Initialized" );
        enabled = true;
    } else {
        debugln( "RTC INIT FAILED!" );
        enabled = false;
    }

    if ( enabled ) {
        readDS3231time();

        logln( "Clock Initialized and set to: %d:%d:%d.", (int)nowHour, (int)nowMinute, (int)nowSecond );
    }
}

void clock::tick( bool twentyfourhour, bool force ) {
    if ( enabled ) {
        if ( force ||
             millis() - nowTimeCycle >=
                 5000 ) // 'DateTime now' takes over 1 milliSecond to return a result, so we don't run it every cycle.
        {
            nowTimeCycle = millis();
            readDS3231time();

            if ( !twentyfourhour && nowHour > 12 ) // 12 or 24 hour
                t.curHour = nowHour - 12;
            else
                t.curHour = nowHour;

            // If the hour or minute are continually updating the other icons flicker\vanish as they are not set
            // constantly, so we flag a minute change to manage display updates.
            if ( prevMinute != nowMinute ) {
                prevMinute = nowMinute;
                t.minuteChange = true;
                t.curMinute = nowMinute;
            }
        }
    }
}

// Convert normal decimal numbers to binary coded decimal
uint8_t clock::decToBcd( uint8_t val ) { return ( ( val / 10 * 16 ) + ( val % 10 ) ); }
// Convert binary coded decimal to normal decimal numbers
uint8_t clock::bcdToDec( uint8_t val ) { return ( ( val / 16 * 10 ) + ( val % 16 ) ); }

void clock::setDS3231time( uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth,
                           uint8_t month, uint8_t year ) {
    // sets time and date data to DS3231
    Wire.beginTransmission( DS3231_I2C_ADDRESS );
    Wire.write( 0 );                      // set next input to start at the seconds register
    Wire.write( decToBcd( second ) );     // set seconds
    Wire.write( decToBcd( minute ) );     // set minutes
    Wire.write( decToBcd( hour ) );       // set hours
    Wire.write( decToBcd( dayOfWeek ) );  // set day of week (1=Sunday, 7=Saturday)
    Wire.write( decToBcd( dayOfMonth ) ); // set date (1 to 31)
    Wire.write( decToBcd( month ) );      // set month
    Wire.write( decToBcd( year ) );       // set year (0 to 99)
    Wire.endTransmission();
}
void clock::readDS3231time() {
    Wire.beginTransmission( DS3231_I2C_ADDRESS );
    Wire.write( 0 ); // set DS3231 register pointer to 00h
    Wire.endTransmission();
    Wire.requestFrom( DS3231_I2C_ADDRESS, 7u );
    // request seven uint8_ts of data from DS3231 starting from register 00h
    nowSecond = bcdToDec( Wire.read() & 0x7f );
    nowMinute = bcdToDec( Wire.read() );
    nowHour = bcdToDec( Wire.read() & 0x3f );
    dayOfWeek = bcdToDec( Wire.read() );
    dayOfMonth = bcdToDec( Wire.read() );
    month = bcdToDec( Wire.read() );
    year = bcdToDec( Wire.read() );
}
