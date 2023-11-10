#include <Arduino.h>
#include "Wire.h"
#include "SPI.h"

#include "buttonPanel.hpp"
#include "confMenu.hpp"
#include "acAmp.hpp"
#include "display.hpp"

void shortButtonAction(btn_enum shortButton);
void longButtonAction(btn_enum longButton);
