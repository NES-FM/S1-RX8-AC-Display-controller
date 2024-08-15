#include "mainMenu.hpp"

String mainMenuFuncPage::get_formatted_text() {
    float res = _func();
    return _name + " " + String( res, _decimals ) + _unit;
}

String mainMenuPtrPage::get_formatted_text() {
    if ( _intptr )
        return _name + " " + String( *_intptr ) + _unit;
    else if ( _floatptr )
        return _name + " " + String( *_floatptr, _decimals ) + _unit;
    return "";
}

float mainMenuPtrPage::getValue() { 
    if (_floatptr) return *_floatptr;
    if (_intptr) return (float)*_intptr;
    return 0.0;
}

void mainMenu_c::registerPage( mainMenuPage *item ) {
    if ( num_el_in_page_vector < page_vector_size ) {
        page_vector[num_el_in_page_vector] = item;
        num_el_in_page_vector++;
    } else {
        logln( "ERROR!!! BG_VECTOR IS NOT BIG ENOUGH" );
        delay( 5000 );
    }
}

void mainMenu_c::setPage( int page_num ) {
    if ( page_num < 0 )
        page_num = num_el_in_page_vector - 1;
    else if ( page_num >= num_el_in_page_vector )
        page_num = 0;
    _cur_page = page_num;
}

void mainMenu_c::setValue( float value ) {
    mainMenuPage *p = page_vector[_cur_page];
    if ( value < p->min_val )
        value = p->max_val;
    else if ( value > p->max_val )
        value = p->min_val;

    p->value = value;
}

String mainMenu_c::draw() {
    if ( millis() - lastMidsectionMillis > 300 ) {
        lastMidsectionMillis = millis();

        midIcons.mid_section_colon = true;

        if ( cur_page_num() == 0 ) {
            midIcons.Dolby = false;

            if ( bat_page->getValue() > 13.5 ) { // Motor Running
                if ( temp_page->getValue() <= 90 ) { // Motor Cold
                    return temp_page->get_formatted_text();
                } else { // Motor Hot
                    midIcons.mid_section_colon = false;
                    return "Mazda RX-8"; // TODO: Calculate fuel consumption + motor Temp (aka OBDII)
                }
            } else { // Motor Off
                return bat_page->get_formatted_text();
            }
        } else {
            midIcons.Dolby = true;
            return getPageText( _cur_page );
        }
    }
    return ""; 
}

String mainMenu_c::getPageText( int page_num ) { return page_vector[page_num]->get_formatted_text(); }
