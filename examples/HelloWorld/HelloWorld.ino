/*
  06/01/2016
  Author: Makerbro
  Platforms: ESP8266
  Language: C++
  File: HelloWorld.ino
  ------------------------------------------------------------------------
  Description: 
  Demo for OLED display -- writing a string.
  ------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  ------------------------------------------------------------------------
  License:
  Released under the MIT license. Please check LICENSE.txt for more
  information.  All text above must be included in any redistribution. 
*/
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>

void setup()
{
  Wire.begin();	
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,1);              // Set cursor position
  oled.putString("   ACROBOTIC   ");
  oled.setTextXY(0,2);              // Set cursor position
  oled.putString("  industries   ");
}

void loop()
{
}
