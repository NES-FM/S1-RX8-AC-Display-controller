#pragma once
#include <Arduino.h>
#include <string.h>

#include "logger.h"

class espComm {
  public:
    espComm( HardwareSerial &SerialPort ) : ser( SerialPort ) {}
    void init();
    void tick();

    enum status { SUCCESS, WAITING, TIMEOUT, FAILURE };

    status storeData( const char *key, void *data, size_t data_size );
    template <typename T> T loadData( const char *key );

    status debugData( const char *to_send );

  private:
    enum messageIds {
        requestStoreData = 0b00000000,
        requestLoadData = 0b00000001,
        sendDebugData = 0b00000010,
        responseOk = 0b01000000,
        responseFailure = 0b01000001
    };

    status sendMessage();

    status checkForMessage();

    status readMessage();

    status handleMessage();

    //                          0x00  0xFF ID LEN
    uint8_t txMessageHeader[4] = { 0x00, 0xFF, 0, 0 };
    uint8_t *txMessageContent;
    uint8_t rxMessageHeader[4] = { 0x00, 0xFF };
    uint8_t *rxMessageContent;

    HardwareSerial &ser;
};
