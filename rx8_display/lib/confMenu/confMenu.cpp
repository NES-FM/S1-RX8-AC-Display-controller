#include "confMenu.hpp"

void confMenu::registerPage(confPage *item)
{
    if (num_el_in_conf_pages < conf_pages_size) {
        conf_pages[num_el_in_conf_pages] = item;
        num_el_in_conf_pages++;
    } else {
        logln("ERROR!!! CONF_PAGES IS NOT BIG ENOUGH");
        delay(5000);
    }
}

void confMenu::shortButtonPress(btn_enum shortButton) {
    switch (shortButton)
    {
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

//TODO: Implement menu long button press
void confMenu::longButtonPress(btn_enum longButton) 
{
    deactivate();
}

void confMenu::changeRotary(int fan, int temp) {
    relValue += fan;
    relValue += temp;
}

void confMenu::activate() 
{
    confMode = true;
    current_page = 0;
    relValue = 0;
    enter = false;
    mainMenu = true;
    midIcons.Dolby = true;
}

// TODO: ConfMenu Deactivate    
void confMenu::deactivate() 
{
    confMode = false;
}

void confMenu::menuTick() 
{
    if (mainMenu) {
        if (relValue > 0) current_page++;
        else if (relValue < 0) current_page--;

        if (current_page == 255) current_page = num_el_in_conf_pages-1;
        else if (current_page >= conf_pages_size) current_page = 0;

        while (!conf_pages[current_page]) {
            current_page++;
            if (current_page == 255) current_page = num_el_in_conf_pages-1;
            else if (current_page >= conf_pages_size) current_page = 0;
        }

        outputText = conf_pages[current_page]->menuName;
    } else {
        confPage* page = conf_pages[current_page];
        page->changeValue(relValue);
        if (enter) {
            if (page->pressEnter()) {
                mainMenu = true;
                menuTick();
                goto end;
            }
        }
        outputText = page->getDisplayText();
    }

end:
    relValue = 0;
    enter = false;
}
