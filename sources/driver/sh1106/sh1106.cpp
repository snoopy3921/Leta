#include "sh1106.h"
#include "io_cfg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h> 
using namespace std;


void SH1106_RST_Clr() 						
{
  GPIO_ResetBits(SH1106_RST_PORT, SH1106_RST_PIN);
}
void SH1106_RST_Set()
{
  GPIO_SetBits(SH1106_RST_PORT, SH1106_RST_PIN);
} 						
void SH1106_DC_Clr() 						
{
  GPIO_ResetBits(SH1106_DCX_PORT, SH1106_DCX_PIN);
}
void SH1106_DC_Set() 						
{
  GPIO_SetBits(SH1106_DCX_PORT, SH1106_DCX_PIN);

}

void SH1106_IO_Init()
{
  oled_hw_init();
}

static void SH1106_WriteByte(uint8_t dat,uint8_t cmd)
{
	if(cmd)
	{
		SH1106_DC_Set();
	}
	else
	{
		SH1106_DC_Clr();
	}
	SPI_I2S_SendData(SPI1, dat);
}
static void SH1106_WriteByteBurst(uint8_t *dat,uint8_t cmd)
{
	if(cmd)
	{
		SH1106_DC_Set();
	}
	else
	{
		SH1106_DC_Clr();
	}
	DMA1_Channel3->CMAR = (uint32_t)dat;
	DMA1_Channel3->CNDTR = (uint32_t)128;
  	DMA1_Channel3->CPAR = (uint32_t) &SPI1->DR;
	DMA_ClearFlag(DMA1_FLAG_TC3 | DMA1_FLAG_GL3);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel3, ENABLE); 
	// Wait until the data is received
    while (!DMA_GetFlagStatus(DMA1_FLAG_TC3));

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE); 


}

static void SH1106_Reset()
{
	SH1106_RST_Set();
	for(volatile int i = 0; i < 100000; i ++);
  SH1106_RST_Clr();
	for(volatile int i = 0; i < 100000; i ++);
	SH1106_RST_Set(); 
}



static uint8_t buffer[SH1106_LCDHEIGHT * SH1106_LCDWIDTH / 8] = {0x00};

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif


#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

typedef enum
{
    HORIZONTAL = 0,
    VERTICAL,
    PAGE,
    INVALID,
    END_MEMORY_ADDRESSING_MODES
} MEMORY_ADDRESSING_MODES;

void sh1106::sh1106_command(uint8_t c)
{
    SH1106_WriteByte(c, OLED_CMD);
}
void sh1106::sh1106_data(uint8_t c)
{
    SH1106_WriteByte(c, OLED_DATA);
}

sh1106::sh1106(){}

sh1106::~sh1106(){}
	
void sh1106::init()
{
  SH1106_IO_Init();
  SH1106_Reset();
  // Init sequence
  sh1106_command(SH1106_DISPLAYOFF);                    // 0xAE
  sh1106_command(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5
	// Init sequence for 128x64 OLED module
	sh1106_command(0xF0);                                 // the suggested ratio 0xF0 XXXXXXXX
	sh1106_command(SH1106_SETMULTIPLEX);                  // 0xA8 XXXXXXXXX
	sh1106_command(0x3F);								   								// XXXXXXXXXXX
	sh1106_command(SH1106_OUTPUT_FOLLOWS_RAM);            // 0xA4 XXXXXXXXX
	sh1106_command(SH1106_SETDISPLAYOFFSET);              // 0xD3 XXXXXXXXX
	sh1106_command(0x0);                                  // no offset  XXXXXXXXX
	sh1106_command(SH1106_SETSTARTLINE | 0x0);            // line #0
	sh1106_command(SH1106_CHARGEPUMP);                    // 0x8D XXXXXXXXXX
	sh1106_command(0x14);
	//sh1106_command(SH1106_MEMORYMODE);                    // 0x20 XXXXXXXXXXXXXX
	sh1106_command(PAGE);                               // 0x2 Paged XXXXXXXXXXX
	sh1106_command(HORIZONTAL);                           // 0x0 Horizontal XXXXXXXXXXX
	sh1106_command(SH1106_SET_PAGE_ADDRESS); 							// start at page address 0 XXXXXXXXX
	//sh1106_command(SH1106_SEGREMAP | 0x1);				   		// ?????????????
	sh1106_command(SH1106_COMSCANDEC);					   				// XXXXXXXXXX
	sh1106_command(SH1106_SETLOWCOLUMN);				   				// XXXXXXXXXX
	sh1106_command(SH1106_SETHIGHCOLUMN);				   				// XXXXXXXXXX
	sh1106_command(SH1106_SETCOMPINS);                    // 0xDA XXXXXXXXX
	sh1106_command(0x12);								   								// XXXXXXXXXX
	sh1106_command(SH1106_SETCONTRAST);                   // 0x81 XXXXXXXX
	sh1106_command(0xCF);
	sh1106_command(SH1106_SET_SEGMENT_REMAP);             // 0xA1 XXXXXXXX
  sh1106_command(SH1106_SETPRECHARGE);                  // 0xd9 XXXXXXXXX
	sh1106_command(0xF1);
	sh1106_command(SH1106_SETVCOMDETECT);                 // 0xDB XXXXXXXX
	sh1106_command(0x20);								   								// 0.77xVcc XXXXXXX
	sh1106_command(SH1106_DISPLAYALLON_RESUME);           // 0xA4 XXXXXXXXXX
	sh1106_command(SH1106_NORMALDISPLAY);                 // 0xA6  XXXXXXXXXX
  clear();
	sh1106_command(SH1106_DISPLAYON);											//--turn on oled panel
}
void sh1106::display_on()
{
  sh1106_command(SH1106_DISPLAYON);	
}
void sh1106::display_off()
{
  sh1106_command(SH1106_DISPLAYOFF);	
}

void sh1106::display_buffer(const uint8_t* buff)
{
	for (int page = 0; page < SH1106_MAX_PAGE_COUNT; page++)
	{
		sh1106_command(SH1106_SET_PAGE_ADDRESS + page);
		sh1106_command(0x02); // low column start address
		sh1106_command(0x10); // high column start address

		SH1106_WriteByteBurst((uint8_t*)&buff[(page << 7)], OLED_DATA);

		
		// for (int pixel = 0; pixel < SH1106_LCDWIDTH; pixel++)
		// {
		// 	sh1106_data(buff[(page << 7) + pixel]); // Page * 128 + pixel
		// }
	}

}


void sh1106::clear(void) {
  memset(buffer, 0, (SH1106_LCDWIDTH*SH1106_LCDHEIGHT/8));
}


void sh1106::invertDisplay(uint8_t i)
{
	if (i) {
    sh1106_command(SH1106_INVERTDISPLAY);
  } else {
    sh1106_command(SH1106_NORMALDISPLAY);
  }
}
void sh1106::setBrightness(unsigned char brightness)	
{
	sh1106_command(SH1106_SETCONTRAST);		//--set contrast control register
	sh1106_command(brightness);						// Set SEG Output Current Brightness 0-255
}