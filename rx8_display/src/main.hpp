#include <Arduino.h>
#include "Wire.h"
#include "SPI.h"

#include "logger.h"
#include "command_parser.h"

#include "buttonPanel.hpp"
#include "confMenu.hpp"
#include "acAmp.hpp"
#include "display.hpp"
#include "clock.hpp"
#include "backlightLed.hpp"

void shortButtonAction(btn_enum shortButton);
void longButtonAction(btn_enum longButton);
void execute_command(String cmd);
