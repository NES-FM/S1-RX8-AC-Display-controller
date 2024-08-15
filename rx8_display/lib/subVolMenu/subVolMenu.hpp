#pragma once

#include "baseMenu.hpp"
#include "dsp.hpp"

class subVolMenu : public baseMenu {
  public:
    dsp* _dsp;
    void setPage( int page_num ) override;
    String draw() override;
    void changeRotary( int fan, int temp ) override;
  private:
    int relValue = 0;
    const float step_size = 0.05f;
};
