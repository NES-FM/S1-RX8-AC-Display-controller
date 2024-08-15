#include "baseMenu.hpp"

bool baseMenu::displayChanged() { 
    bool r = _hasChanged;
    _hasChanged = false;
    return r;
}
