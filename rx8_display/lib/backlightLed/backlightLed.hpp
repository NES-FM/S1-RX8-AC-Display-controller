#pragma once

#include "pins.h"
#include "logger.h"

class backlightLedBase {
    public:
        backlightLedBase() {};
        virtual void init() {};
        virtual void setValue(bool on, uint16_t brightness) {}
        int _pin;
    protected:
};

class digitalBacklightLed : public backlightLedBase {
    public:
        digitalBacklightLed(int pin);
        void init() override;
        void setValue(bool on, uint16_t brightness) override;
};

class pwmBacklightLed : public backlightLedBase {
    public:
        pwmBacklightLed(int pin);
        void init() override;
        void setValue(bool on, uint16_t brightness) override;
    private:
        bool enabled = true;
};

class backlightLedManager {
    public:
        backlightLedManager();
        void registerBackgroundLed(backlightLedBase* item);
        void init();
        void tick();
    private:
        backlightLedBase* bg_vector[10];
        uint8_t num_el_in_bg_vector = 0;
        unsigned long lastBacklightMillis;
};
