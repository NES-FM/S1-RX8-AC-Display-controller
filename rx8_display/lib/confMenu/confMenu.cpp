#include "confMenu.hpp"

void confMenu::registerPage( confPage *item ) {
    if ( num_el_in_conf_pages < conf_pages_size ) {
        conf_pages[num_el_in_conf_pages] = item;
        num_el_in_conf_pages++;
    } else {
        logln( "ERROR!!! CONF_PAGES IS NOT BIG ENOUGH" );
        delay( 5000 );
    }
}

void confMenu::shortButtonPress( btn_enum shortButton ) {
    switch ( shortButton ) {
    case Auto:
        enter = true;
        break;
    case Mode:
        enter = true;
        break;
    case AC:
        relValue++;
        break;
    case frontDemist:
        relValue--;
        break;
    case rearDemist:
        relValue++;
        break;
    case AirSource:
        relValue--;
        break;
    case Off:
        enter = true;
        break;
    default:
        break;
    }
}

void confMenu::changeRotary( int fan, int temp ) {
    relValue += fan;
    relValue += temp;
}

void confMenu::setPage( int page_num ) {
    if ( page_num < 0 )
        page_num = num_el_in_conf_pages - 1;
    else if ( page_num >= num_el_in_conf_pages )
        page_num = 0;
    _cur_page = page_num;
}

void confMenu::activate() {
    baseMenu::activate();
    current_page = 0;
    relValue = 0;
    enter = false;
    confRootMenu = true;
    midIcons.Dolby = true;
}


String confMenu::draw() {
    String outputText;
    if ( confRootMenu ) {
        if ( relValue > 0 )
            next();
        else if ( relValue < 0 )
            previous();

        outputText = conf_pages[current_page]->menuName;
    } else {
        confPage *page = conf_pages[current_page];
        page->changeValue( relValue );
        if ( enter ) {
            if ( page->pressEnter() ) {
                confRootMenu = true;
                outputText = draw(); // Draw root menu instead, as page wants to exit
                goto end;
            }
        }
        outputText = page->getDisplayText();
    }

end:
    relValue = 0;
    enter = false;
    return outputText;
}
