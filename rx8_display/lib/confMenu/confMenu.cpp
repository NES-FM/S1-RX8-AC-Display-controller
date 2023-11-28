#include "confMenu.hpp"

//TODO: Implement menu short button press
void confMenu::shortButtonPress(btn_enum shortButton) {}

//TODO: Implement menu long button press
void confMenu::longButtonPress(btn_enum longButton) 
{
    deactivate();
}

//TODO: Implement menu rotary switches
void confMenu::changeRotary(rotationDir fan, rotationDir temp) {}

//TODO: ConfMenu Activate
void confMenu::activate() 
{
    confMode = true;
    // menuIndex = 1;
}

// TODO: ConfMenu Deactivate    
void confMenu::deactivate() 
{
    confMode = false;
    // acAmpOn = true;
    // acAmpRxChanged = true;
    // acAmpMessageToProcess = true;
    // menuValue = 0;
    // menuStartValue = false;
}

void confMenu::menuTick() 
{

}
