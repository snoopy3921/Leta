#ifndef __SH1106_H__
#define __SH1106_H__
#include "stm32f10x.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



#define OLED_CMD     0	
#define OLED_DATA    1

#define BLACK 0
#define WHITE 1
#define INVERSE 2

//common parameters
#define WIDTH 128
#define HEIGHT 64
#define FBSIZE 1024 //128x8
#define MAXROW 8

#define SH1106_LCDWIDTH                  128
#define SH1106_LCDHEIGHT                 64
#define SH1106_MAX_PAGE_COUNT							8

#define SH1106_SETCONTRAST 0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON 0xA5
#define SH1106_NORMALDISPLAY 0xA6
#define SH1106_INVERTDISPLAY 0xA7
#define SH1106_DISPLAYOFF 0xAE
#define SH1106_DISPLAYON 0xAF
#define SH1106_OUTPUT_FOLLOWS_RAM 0xA4


#define SH1106_SETDISPLAYOFFSET 0xD3
#define SH1106_SETCOMPINS 0xDA

#define SH1106_SETVCOMDETECT 0xDB

#define SH1106_SETDISPLAYCLOCKDIV 0xD5
#define SH1106_SETPRECHARGE 0xD9

#define SH1106_SETMULTIPLEX 0xA8

#define SH1106_SETLOWCOLUMN 0x00
#define SH1106_SETHIGHCOLUMN 0x10
#define SH1106_SET_SEGMENT_REMAP	0xA1 // 0 to 127

#define SH1106_SETSTARTLINE 0x40

#define SH1106_MEMORYMODE 0x20
#define SH1106_COLUMNADDR 0x21
#define SH1106_PAGEADDR   0x22
#define SH1106_SET_PAGE_ADDRESS	0xB0 /* sets the page address from 0 to 7 */

#define SH1106_COMSCANINC 0xC0
#define SH1106_COMSCANDEC 0xC8

#define SH1106_SEGREMAP 0xA0

#define SH1106_CHARGEPUMP 0x8D

#define SH1106_EXTERNALVCC 0x1
#define SH1106_SWITCHCAPVCC 0x2


class sh1106
{
public:
    sh1106();
    ~sh1106();
		void init();
		void clear(void);
    void invertDisplay(uint8_t i);
    void display_on();
    void display_off();
    void display_buffer(const uint8_t* buff);
    void setBrightness(unsigned char brightness)	;
private:
    void sh1106_command(uint8_t c);
    void sh1106_data(uint8_t c);
};


#endif