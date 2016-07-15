/*
  06/01/2016
  Author: Makerbro
  Platforms: ESP8266
  Language: C++
  File: ACROBOTIC_SSD1306.cpp
  ------------------------------------------------------------------------
  Description: 
  SSD1306 OLED Driver Library.
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

#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#include "Wire.h"
#include "ACROBOTIC_SSD1306.h"
#include "fonts/font8x8.h"

void ACROBOTIC_SSD1306::init(void)
{
  sendCommand(0xAE);            //display off
  sendCommand(0xA6);            //Set Normal Display (default)
  sendCommand(0xAE);            //DISPLAYOFF
  sendCommand(0xD5);            //SETDISPLAYCLOCKDIV
  sendCommand(0x80);            // the suggested ratio 0x80
  sendCommand(0xA8);            //SSD1306_SETMULTIPLEX
  sendCommand(0x3F);
  sendCommand(0xD3);            //SETDISPLAYOFFSET
  sendCommand(0x0);             //no offset
  sendCommand(0x40|0x0);        //SETSTARTLINE
  sendCommand(0x8D);            //CHARGEPUMP
  sendCommand(0x14);
  sendCommand(0x20);            //MEMORYMODE
  sendCommand(0x00);            //0x0 act like ks0108
  sendCommand(0xA1);            //SEGREMAP   Mirror screen horizontally (A0)
  sendCommand(0xC8);            //COMSCANDEC Rotate screen vertically (C0)
  sendCommand(0xDA);            //0xDA
  sendCommand(0x12);            //COMSCANDEC
  sendCommand(0x81);            //SETCONTRAST
  sendCommand(0xCF);            //
  sendCommand(0xd9);            //SETPRECHARGE 
  sendCommand(0xF1); 
  sendCommand(0xDB);            //SETVCOMDETECT                
  sendCommand(0x40);
  sendCommand(0xA4);            //DISPLAYALLON_RESUME        
  sendCommand(0xA6);            //NORMALDISPLAY             
  clearDisplay();
  sendCommand(0x2E);            // stop scroll
  sendCommand(0x20);            //Set Memory Addressing Mode
  sendCommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
}

void ACROBOTIC_SSD1306::sendCommand(unsigned char command)
{
  Wire.beginTransmission(SSD1306_Address);    // begin I2C communication
  Wire.write(SSD1306_Command_Mode);           // Set OLED Command mode
  Wire.write(command);
  Wire.endTransmission();                       // End I2C communication
}

void ACROBOTIC_SSD1306::setBrightness(unsigned char Brightness)
{
   sendCommand(SSD1306_Set_Brightness_Cmd);
   sendCommand(Brightness);
}

void ACROBOTIC_SSD1306::setHorizontalMode()
{
    addressingMode = HORIZONTAL_MODE;
    sendCommand(0x20);                      //set addressing mode
    sendCommand(0x00);                      //set horizontal addressing mode
}

void ACROBOTIC_SSD1306::setPageMode()
{
    addressingMode = PAGE_MODE;
    sendCommand(0x20);                      //set addressing mode
    sendCommand(0x02);                      //set page addressing mode
}

void ACROBOTIC_SSD1306::setTextXY(unsigned char Row, unsigned char Column)
{
    sendCommand(0xB0 + Row);                    //set page address
    sendCommand(0x00 + (8*Column & 0x0F));      //set column lower address
    sendCommand(0x10 + ((8*Column>>4)&0x0F));   //set column higher address
}

void ACROBOTIC_SSD1306::clearDisplay()
{
  unsigned char i,j;
  sendCommand(SSD1306_Display_Off_Cmd);     //display off
  for(j=0;j<8;j++)
  {    
    setTextXY(j,0);    
    {
      for(i=0;i<16;i++)  //clear all columns
      {
        putChar(' ');    
      }
    }
  }
  sendCommand(SSD1306_Display_On_Cmd);     //display on
  setTextXY(0,0);    
}

void ACROBOTIC_SSD1306::sendData(unsigned char Data)
{
     Wire.beginTransmission(SSD1306_Address); // begin I2C transmission
     Wire.write(SSD1306_Data_Mode);            // data mode
     Wire.write(Data);
     Wire.endTransmission();                    // stop I2C transmission
}

void ACROBOTIC_SSD1306::putChar(unsigned char C)
{
    if(C < 32 || C > 127) //Ignore non-printable ASCII characters. This can be modified for multilingual font.
    {
    C=' '; //Space
    }    
    unsigned char i=0;
    for(i=0;i<8;i++)
    {
       //read bytes from code memory
       sendData(pgm_read_byte(&BasicFont[C-32][i])); //font array starts at 0, ASCII starts at 32. Hence the translation
    }
}

void ACROBOTIC_SSD1306::putString(const char *String)
{
    unsigned char i=0;
    while(String[i])
    {
        putChar(String[i]);     
        i++;
    }
}

unsigned char ACROBOTIC_SSD1306::putNumber(long long_num)
{
  unsigned char char_buffer[10]="";
  unsigned char i = 0;
  unsigned char f = 0;

  if (long_num < 0) 
  {
    f=1;
    putChar('-');
    long_num = -long_num;
  } 
  else if (long_num == 0) 
  {
    f=1;
    putChar('0');
    return f;
  } 

  while (long_num > 0) 
  {
    char_buffer[i++] = long_num % 10;
    long_num /= 10;
  }

  f=f+i;
  for(; i > 0; i--)
  {
    putChar('0'+ char_buffer[i - 1]);
  }
  return f;

}

unsigned char ACROBOTIC_SSD1306::putFloat(float floatNumber,unsigned char decimal)
{
  unsigned int temp=0;
  float decy=0.0;
  float rounding = 0.5;
  unsigned char f=0;
  if(floatNumber<0.0)
  {
    putString("-");
    floatNumber = -floatNumber;
    f +=1;
  }
  for (unsigned char i=0; i<decimal; ++i)
  {
    rounding /= 10.0;
  }
    floatNumber += rounding;
  
  temp = floatNumber;
  f += putNumber(temp);
  if(decimal>0)
  {
    putChar('.');
    f +=1;
 }
  decy = floatNumber-temp;//decimal part, 
  for(unsigned char i=0;i<decimal;i++)//4 
  {
    decy *=10;// for the next decimal
    temp = decy;//get the decimal
    putNumber(temp);
    decy -= temp;
  }
  f +=decimal;
  return f;
}
unsigned char ACROBOTIC_SSD1306::putFloat(float floatNumber)
{
  unsigned char decimal=2;
  unsigned int temp=0;
  float decy=0.0;
  float rounding = 0.5;
  unsigned char f=0;
  if(floatNumber<0.0)
  {
    putString("-");
    floatNumber = -floatNumber;
    f +=1;
  }
  for (unsigned char i=0; i<decimal; ++i)
  {
    rounding /= 10.0;
  }
    floatNumber += rounding;
  
  temp = floatNumber;
  f += putNumber(temp);
  if(decimal>0)
  {
    putChar('.');
    f +=1;
 }
  decy = floatNumber-temp;//decimal part, 
  for(unsigned char i=0;i<decimal;i++)//4 
  {
    decy *=10;// for the next decimal
    temp = decy;//get the decimal
    putNumber(temp);
    decy -= temp;
  }
  f +=decimal;
  return f;
}

void ACROBOTIC_SSD1306::drawBitmap(unsigned char *bitmaparray,int bytes)
{
  char localAddressMode = addressingMode;
  if(addressingMode != HORIZONTAL_MODE)
  {
      //Bitmap is drawn in horizontal mode     
      setHorizontalMode();
  }

  for(int i=0;i<bytes;i++)
  {
      sendData(pgm_read_byte(&bitmaparray[i]));
  }

  if(localAddressMode == PAGE_MODE)
  {
     //If pageMode was used earlier, restore it.
     setPageMode(); 
  }
  
}

void ACROBOTIC_SSD1306::setHorizontalScrollProperties(bool direction,unsigned char startPage, unsigned char endPage, unsigned char scrollSpeed)
{
/*
Use the following defines for 'direction' :

 Scroll_Left            
 Scroll_Right            

Use the following defines for 'scrollSpeed' :

 Scroll_2Frames        
 Scroll_3Frames
 Scroll_4Frames
 Scroll_5Frames    
 Scroll_25Frames
 Scroll_64Frames
 Scroll_128Frames
 Scroll_256Frames

*/

   if(Scroll_Right == direction)
   {
        //Scroll Right
        sendCommand(0x26);
   }
   else
   {
        //Scroll Left  
        sendCommand(0x27);

   }
    sendCommand(0x00);
    sendCommand(startPage);
    sendCommand(scrollSpeed);
    sendCommand(endPage);
    sendCommand(0x00);
    sendCommand(0xFF);
}

void ACROBOTIC_SSD1306::activateScroll()
{
    sendCommand(SSD1306_Activate_Scroll_Cmd);
}

void ACROBOTIC_SSD1306::deactivateScroll()
{
    sendCommand(SSD1306_Dectivate_Scroll_Cmd);
}

void ACROBOTIC_SSD1306::setNormalDisplay()
{
    sendCommand(SSD1306_Normal_Display_Cmd);
}

void ACROBOTIC_SSD1306::setInverseDisplay()
{
    sendCommand(SSD1306_Inverse_Display_Cmd);
}


ACROBOTIC_SSD1306 oled;  // Preinstantiate Objects

