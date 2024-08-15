#include "dsp.hpp"


dsp::dsp( uint8_t wp_pin ) : wp_pin(wp_pin) {
    // The third parameter is the sample rate
    // An optional fourth parameter is the pin to physically reset the DSP
    _dsp = new SigmaDSP(Wire, DSP_I2C_ADDRESS, 48000.00f /*,12*/);
    // The third parameter is the EEPROM size in kilobits (kb)
    // An optional fourth parameter is the pin to toggle while writing content to EEPROM
    _ee = new DSPEEPROM(Wire, EEPROM_I2C_ADDRESS, 256/*, LED_BUILTIN*/);
}

void dsp::init() {
    pinMode(wp_pin, INPUT);
    _dsp->begin();
    _ee->begin();
    async_delay(1000);
}

void dsp::tick() {
    // If it hasnt initialized and the safety delay is done
    if (!hasInitialized && async_delay(0)) {
        hasInitialized = true;
        lateInit();
    } 
    
    if (!hasInitialized || !enabled) return;

    // Update sub volume only if it has changed
    if (last_sub_vol != sub_vol) {
        _dsp->volume_slew(MOD_VOL_SUB_ALG0_TARGET_ADDR, sub_vol, 12);
        last_sub_vol = sub_vol;
    }
}

void dsp::set_sub_volume( float volume ) {
    sub_vol = (constrain(volume, 0.0f, 1.0f) * 50.0f) - 50.0f;
}
float dsp::get_sub_volume() { 
    return (sub_vol + 50.0f) / 50.0;
}

void dsp::lateInit() {
    logln("Pinging i2c lines...\t0 -> deveice is present\t2 -> device is not present");
    int dsp_res = _dsp->ping();
    int ee_res = _ee->ping();
    logln("DSP response: %d", dsp_res);
    logln("EEPROM ping:  %d", ee_res);

    enabled = (dsp_res == 0) && (ee_res == 0);

    if (!enabled) {
        logln("DSP and/or EEPROM not responding.");
        return;
    }

    // Firmware need to be updated (stalls Arduino)
    if (_ee->getFirmwareVersion() != SIGMA_FW_VERSION) {
        pinMode(wp_pin, OUTPUT); digitalWrite(wp_pin, LOW);
        delay(200);

        log_inline_begin();
        log_inline("Flashing firmware of DSP EEPROM (could take a minute)");
        log_inline(_ee->writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), SIGMA_FW_VERSION) == 1 ? "  |  Success!" : "  |  FAILED!");
        log_inline_end();

        _dsp->reset();
        pinMode(wp_pin, INPUT);

        delay(1000); // Wait for the FW to load from the EEPROM
    }

    secondOrderEQ_t lp;
    lp.filterType = parameters::filterType::butterworthLowpass;
    lp.freq = 100;
    _dsp->EQsecondOrder(MOD_GENFILTER2_ALG0_STAGE0_B0_ADDR, lp);

    secondOrderEQ_t hp;
    hp.filterType = parameters::filterType::butterworthHighpass;
    hp.freq = 100;
    _dsp->EQsecondOrder(MOD_GENFILTER1_ALG0_STAGE0_B0_ADDR, hp);

    _dsp->audioDelay(MOD_DELAY_CH_1_DELAYAMT_ADDR, 0.1);
    _dsp->audioDelay(MOD_DELAY_CH_2_DELAYAMT_ADDR, 0.1);
    _dsp->audioDelay(MOD_DELAY_SUB_DELAYAMT_ADDR, 0.1);

    _dsp->gain(MOD_GAIN_SUB_GAIN1940ALGNS2_ADDR, 2);
    _dsp->volume_slew(MOD_VOL_SUB_ALG0_TARGET_ADDR, -50, 12);

    _dsp->muteADC(false);
    _dsp->muteDAC(false);
    logln("DSP Initialized!");
}

bool dsp::async_delay( unsigned long ms ) {
    if (async_delay_target < 0) async_delay_target = millis() + ms;
    if (async_delay_target <= millis()) {
        async_delay_target = 0;
        return true;
    }
    return false;
}
