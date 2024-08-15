#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "logger.h"

#include "SigmaDSP.h"
#include "SigmaDSP_parameters.h"

#define SIGMA_FW_VERSION 6

class dsp {
  public:
    dsp(uint8_t wp_pin);
    void init();
    void tick();

    // @param volume: value between 0 (mute) and 1 (full)
    void set_sub_volume(float volume);
    float get_sub_volume();
  private:
    SigmaDSP* _dsp;
    DSPEEPROM* _ee;
    uint8_t wp_pin;

    bool enabled = false;

    float sub_vol = -20.0, last_sub_vol = 0.0;

    bool hasInitialized = false;

    void lateInit();
    
    unsigned long async_delay_target = 0;
    bool async_delay(unsigned long ms);
};
