#pragma once

#include "logger.h"
#include <Arduino.h>

class menuPage {
  public:
    virtual String get_formatted_text() { return ""; };
    float value = 0;
    float min_val = 0;
    float max_val = 0;
};

typedef float ( *vFunctionCall )();
class value_func_page : public menuPage {
  public:
    value_func_page( String name, vFunctionCall func, String unit, unsigned int num_decimals = 2 )
        : _name( name ), _func( func ), _unit( unit ), _decimals( num_decimals ) {}
    String get_formatted_text() override;

  private:
    String _name;
    vFunctionCall _func;
    String _unit;
    unsigned int _decimals;
};

class value_ptr_page : public menuPage {
  public:
    value_ptr_page( String name, int *intptr, String unit )
        : _name( name ), _intptr( intptr ), _floatptr( nullptr ), _unit( unit ), _decimals( 0 ) {}
    value_ptr_page( String name, float *floatptr, String unit, unsigned int num_decimals = 2 )
        : _name( name ), _intptr( nullptr ), _floatptr( floatptr ), _unit( unit ), _decimals( num_decimals ) {}
    String get_formatted_text() override;

  private:
    String _name;
    int *_intptr = nullptr;
    float *_floatptr = nullptr;
    String _unit;
    unsigned int _decimals;
};

class menu {
  public:
    void registerPage( menuPage *item );

    void next() { setPage( _cur_page + 1 ); }
    void previous() { setPage( _cur_page - 1 ); }
    void setPage( int page_num );
    int cur_page_num() { return _cur_page; }

    void increment( float step = 1.0 ) { setValue( getValue() + step ); }
    void decrement( float step = 1.0 ) { setValue( getValue() - step ); }
    void setValue( float value );
    float getValue() { return page_vector[_cur_page]->value; }

    String draw();
    String drawPage( int page_num );

  private:
    const static uint8_t page_vector_size = 10;
    menuPage *page_vector[page_vector_size];
    uint8_t num_el_in_page_vector = 0;

    int _cur_page = 0;

    float _cur_value = 0;
};
