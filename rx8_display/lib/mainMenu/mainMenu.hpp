#pragma once

#include "logger.h"
#include <Arduino.h>

#include "baseMenu.hpp"

class mainMenuPage {
  public:
    virtual String get_formatted_text() { return ""; };
    virtual float getValue() { return 0; };
    float value = 0;
    float min_val = 0;
    float max_val = 0;
};

typedef float ( *vFunctionCall )();
class mainMenuFuncPage : public mainMenuPage {
  public:
    mainMenuFuncPage( String name, vFunctionCall func, String unit, unsigned int num_decimals = 2 )
        : _name( name ), _func( func ), _unit( unit ), _decimals( num_decimals ) {}
    String get_formatted_text() override;
    float getValue() override { return _func(); }

  private:
    String _name;
    vFunctionCall _func;
    String _unit;
    unsigned int _decimals;
};

class mainMenuPtrPage : public mainMenuPage {
  public:
    mainMenuPtrPage( String name, int *intptr, String unit )
        : _name( name ), _intptr( intptr ), _floatptr( nullptr ), _unit( unit ), _decimals( 0 ) {}
    mainMenuPtrPage( String name, float *floatptr, String unit, unsigned int num_decimals = 2 )
        : _name( name ), _intptr( nullptr ), _floatptr( floatptr ), _unit( unit ), _decimals( num_decimals ) {}
    String get_formatted_text() override;
    float getValue() override;

  private:
    String _name;
    int *_intptr = nullptr;
    float *_floatptr = nullptr;
    String _unit;
    unsigned int _decimals;
};

class mainMenu_c : public baseMenu {
  public:
    void registerPage( mainMenuPage *item );

    void increment( float step = 1.0 ) { setValue( getValue() + step ); }
    void decrement( float step = 1.0 ) { setValue( getValue() - step ); }
    void setValue( float value );
    float getValue() { return page_vector[_cur_page]->value; }
    
    void setPage( int page_num ) override;

    String draw() override;
    String getPageText( int page_num );

    mainMenuPage* bat_page;
    mainMenuPage* temp_page;

  private:
    const static uint8_t page_vector_size = 10;
    mainMenuPage *page_vector[page_vector_size];
    uint8_t num_el_in_page_vector = 0;

    unsigned long lastMidsectionMillis = 0;

    float _cur_value = 0;
};
