#include "subVolMenu.hpp"

void subVolMenu::setPage( int page_num ) {}

String subVolMenu::draw() { 
    float cur_vol = _dsp->get_sub_volume();
    _dsp->set_sub_volume(cur_vol + ((float)relValue * step_size));
    relValue = 0;
    
    midIcons.mid_section_colon = true;
    return "SUB " + String(_dsp->get_sub_volume() * 100.0f, 0) + "%";
}

void subVolMenu::changeRotary( int fan, int temp ) {
    relValue += fan;
    relValue += temp;
}
