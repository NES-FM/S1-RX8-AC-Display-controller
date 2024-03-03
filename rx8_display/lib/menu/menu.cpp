#include "menu.hpp"

String value_func_page::get_formatted_text() {
    float res = _func();
    return _name + " " + String( res, _decimals ) + _unit;
}

String value_ptr_page::get_formatted_text() {
    if ( _intptr )
        return _name + " " + String( *_intptr ) + _unit;
    else if ( _floatptr )
        return _name + " " + String( *_floatptr, _decimals ) + _unit;
    return "";
}

void menu::registerPage( menuPage *item ) {
    if ( num_el_in_page_vector < page_vector_size ) {
        page_vector[num_el_in_page_vector] = item;
        num_el_in_page_vector++;
    } else {
        logln( "ERROR!!! BG_VECTOR IS NOT BIG ENOUGH" );
        delay( 5000 );
    }
}

void menu::setPage( int page_num ) {
    if ( page_num < 0 )
        page_num = num_el_in_page_vector - 1;
    else if ( page_num >= num_el_in_page_vector )
        page_num = 0;
    _cur_page = page_num;
}

void menu::setValue( float value ) {
    menuPage *p = page_vector[_cur_page];
    if ( value < p->min_val )
        value = p->max_val;
    else if ( value > p->max_val )
        value = p->min_val;

    p->value = value;
}

String menu::draw() { return drawPage( _cur_page ); }

String menu::drawPage( int page_num ) { return page_vector[page_num]->get_formatted_text(); }
