#pragma once
#include <Arduino.h>

#include "clock.hpp"
#include "data_types.h"
#include "logger.h"

class confPage {
  public:
    confPage() {}
    virtual void changeValue( int relValueChange ) {}
    virtual bool pressEnter() { return true; } // Return true if exit back to settings select
    virtual String getDisplayText() { return ""; }
    String menuName = "";

  private:
};

class confMenu {
  public:
    void registerPage( confPage *item );

    void activate();
    void deactivate();

    void menuTick();

    void shortButtonPress( btn_enum shortButton );
    void longButtonPress( btn_enum longButton );
    void changeRotary( int fan, int temp );

    bool confMode = false;
    bool twentyFourHour = true;
    bool displayChanged = false;
    acShow icons;
    midsectionIcons midIcons;
    String outputText;

  private:
    const static uint8_t conf_pages_size = 10;
    confPage *conf_pages[conf_pages_size];
    uint8_t num_el_in_conf_pages = 0;
    uint8_t current_page = 0;
    int relValue = 0;
    bool enter = false;
    bool mainMenu = true;
};
