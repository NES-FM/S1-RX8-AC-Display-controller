#include "espComm.hpp"

void espComm::init() {
    ser.begin( 115200 );
    while ( ser.available() > 0 ) {
        ser.read();
        delay( 5 );
    }
}

void espComm::tick() {
    if ( readMessage() == SUCCESS ) {
        handleMessage();
    }
}

espComm::status espComm::storeData( const char *key, void *data, size_t data_size ) {
    uint8_t keylen = strlen( key ) + 1;
    txMessageHeader[2] = messageIds::requestStoreData;
    txMessageHeader[3] = keylen + data_size + 4;

    txMessageContent = new uint8_t[keylen + data_size];
    memcpy( txMessageContent, key, keylen );
    memcpy( txMessageContent + keylen, &data, data_size );

    status s = sendMessage();

    delete txMessageContent;

    if ( s != SUCCESS )
        return FAILURE;

    unsigned long start = millis();
    while ( checkForMessage() == WAITING ) {
        if ( millis() - start > 3000 )
            return TIMEOUT;
    }

    if ( readMessage() != SUCCESS )
        return FAILURE;

    if ( rxMessageHeader[2] == responseFailure )
        return FAILURE;
    else if ( rxMessageHeader[2] != responseOk )
        handleMessage();

    return SUCCESS;
}

espComm::status espComm::debugData( const char *to_send ) {
    size_t len = strlen( to_send );
    txMessageHeader[2] = messageIds::sendDebugData;
    txMessageHeader[3] = len + 4;

    txMessageContent = (uint8_t *)to_send;

    status s = sendMessage();

    if ( s != SUCCESS )
        return FAILURE;

    return SUCCESS;
}

espComm::status espComm::sendMessage() {
    ser.write( txMessageHeader, 4 );
    ser.write( txMessageContent, txMessageHeader[3] - 4 );
    return SUCCESS;
}

espComm::status espComm::checkForMessage() {
    if ( ser.available() >= 4 ) {
        // debugln("Message from ESP Received");
        return SUCCESS;
    }
    return WAITING;
}

espComm::status espComm::readMessage() {
    status s = checkForMessage();

    if ( s != SUCCESS )
        return s;

    int i = 0;
    int ser_not_av_count = 0;
    rxMessageHeader[3] = 99;
    delete rxMessageContent;
    while ( i < rxMessageHeader[3] ) {
        // If no data is available, wait a bit, it might just not be there yet
        while (!ser.available() && ser_not_av_count < 200) {
            delay(1);
            ser_not_av_count++;
        }
        if (ser_not_av_count >= 200) break;
        ser_not_av_count = 0;

        if ( i < 4 ) {
            rxMessageHeader[i] = ser.read();
            // debugln("%d: %d", i, (int)rxMessageHeader[i]);
            if ( i == 0 && rxMessageHeader[0] != 0x00 )
                continue;
            else if ( i == 1 && rxMessageHeader[1] != 0xFF ) {
                i = 0;
                continue;
            } else if ( i == 3 )
                rxMessageContent = new uint8_t[rxMessageHeader[3]];
        } else {
            rxMessageContent[i - 4] = ser.read();
        }

        i++;
    }
    if ( i != rxMessageHeader[3] ) {
        debugln("ERROR: i=%d != len=%d", i, rxMessageHeader[3]);
        debug_inline_begin();
        debug_inline("Received data: ");
        for (int jk = 0; jk < rxMessageHeader[3]; jk++)
            debug_inline("%x", (char)rxMessageContent[jk]);
        debug_inline_end();
        return FAILURE;
    }

    return SUCCESS;
}

espComm::status espComm::handleMessage() {
    messageIds id = (messageIds)rxMessageHeader[2];
    if ( id == requestLoadData ) {
        logln( "requestLoadData" );
    } else if ( id == sendDebugData ) {
        char *str = (char *)memcpy( malloc( rxMessageHeader[3] - 3 ), rxMessageContent, rxMessageHeader[3] - 4 );
        str[rxMessageHeader[3] - 4] = 0;
        log_esp( "%s\r\n", str );
        delete str;
    } else if ( id == responseOk ) {
        logln( "responseOk" );
    } else if ( id == responseFailure ) {
        logln( "responseFailure" );
    } else if ( id == requestStoreData ) {
        size_t keylen = strlen( (const char *)rxMessageContent );
        char *key = (char *)malloc( keylen + 1 );
        strcpy( key, (const char *)rxMessageContent );

        size_t datalen = rxMessageHeader[3] - 5 - keylen;
        uint8_t *data = (uint8_t *)malloc( datalen );
        memcpy( data, (const uint8_t *)( rxMessageContent + keylen + 1 ), datalen );

        log_inline_begin();
        log_inline( "requestStoreData: keylen: %d, key: \"%s\", datalen: %d, data:", keylen, key, datalen );
        for ( unsigned int i = 0; i < datalen; i++ ) {
            log_inline( " %#04x", data[i] );
        }
        log_inline_end();

        delete key;
        delete data;
    } else {
        logln( "default reached with id %d=%d", (messageIds)rxMessageHeader[2], id );
        return FAILURE;
    }

    return SUCCESS;
}
