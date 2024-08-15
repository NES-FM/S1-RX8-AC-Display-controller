#pragma once
#include <Arduino.h>

#include "baseMenu.hpp"

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

class confMenu : public baseMenu {
  public:
    void registerPage( confPage *item );

    void activate() override;

    void shortButtonPress( btn_enum shortButton ) override;
    void changeRotary( int fan, int temp ) override;

    void setPage( int page_num ) override;
    String draw() override;
    
    bool twentyFourHour = true;

  private:
    const static uint8_t conf_pages_size = 10;
    confPage *conf_pages[conf_pages_size];
    uint8_t num_el_in_conf_pages = 0;
    uint8_t current_page = 0;
    int relValue = 0;
    bool enter = false;
    bool confRootMenu = true;
};
