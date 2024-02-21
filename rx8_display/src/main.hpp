#include "SPI.h"
#include "Smoothed.h"
#include "Wire.h"
#include <Arduino.h>

#include "command_parser.h"
#include "logger.h"

#include "acAmp.hpp"
#include "backlightLed.hpp"
#include "buttonPanel.hpp"
#include "clock.hpp"
#include "confMenu.hpp"
#include "display.hpp"
#include "espComm.hpp"

void shortButtonAction( btn_enum shortButton );
void longButtonAction( btn_enum longButton );
void midsectionHandler();
void execute_command( String cmd );
