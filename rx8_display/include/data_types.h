#pragma once

enum btn_enum {
    no_button = 0,
    Auto = 1,        // Auto switch
    Mode = 2,        // Mode switch
    AC = 3,          // A/C switch
    frontDemist = 4, // frontDemist switch
    rearDemist = 5,  // rearDemist switch
    AirSource = 6,   // AirSource switch
    _invalid = 7,    // Error if we get here with RX8
    Off = 8          // Off switch
};
struct buttonState {
    int fanRotation;
    int tempRotation;
    btn_enum shortPushButton;
    btn_enum longPushButton;
};

struct acShow {
    uint8_t fanSpeed;
    uint8_t tempDigits[3];
    bool displayAmbient;
    bool stateAuto;
    bool stateAc;
    bool stateEco;
    bool modeFrontDemist;
    bool modeRearDemist;
    bool modeRecirculate;
    bool modeFeet;
    bool modeFace;
    bool ampOn;
    bool ampRunning;
};

struct timeObj {
    uint8_t curMinute;
    uint8_t curHour;
    bool minuteChange;
};

struct midsectionIcons {
    bool CD_IN;
    bool MD_IN;
    bool ST;
    bool Dolby;
    bool AF;
    bool PTY;
    bool RPT;
    bool RDM;
    bool TA;
    bool TP;
    bool Auto_M;
    bool mid_section_colon;
    bool fullstop_char_10_11;
    bool fullstop_char_11_12;
    bool min_sec_prime_marks;
};
