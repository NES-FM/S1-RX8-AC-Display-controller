#pragma once
#include <Arduino.h>

#include "data_types.h"

class baseMenu {
  public:
    bool isActive() const { return _isActive; }
    virtual void activate() { _isActive = true; }
    virtual void deactivate() { _isActive = false; }
    
    void next() { setPage( _cur_page + 1 ); }
    void previous() { setPage( _cur_page - 1 ); }
    virtual void setPage( int page_num ) {}
    int cur_page_num() { return _cur_page; }

    virtual String draw() { return ""; }

    virtual void shortButtonPress( btn_enum shortButton ) {}
    virtual void changeRotary( int fan, int temp ) {}

    bool displayChanged();
    acShow icons;
    midsectionIcons midIcons;

  protected:
    int _cur_page = 0;
    bool _isActive = false;
    bool _hasChanged = false;
};
