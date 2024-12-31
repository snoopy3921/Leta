#include "gui_render.h"
#include "string.h"
#include "gui_port.h"
#include "gui_cfg.h"
#include "gui_font.h"
#include <stdarg.h>
#include <stdio.h>
#include <math.h> 

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif


uint8_t gui_scr_buffer[GUI_CFG_DISPLAY_W * GUI_CFG_DISPLAY_H / 8] = {0};


/** 
 * Description: Write 1 byte into buffer with styles
 * 
 * @param {x} x coordinate of byte
 * @param {y} y coordinate of byte
 * @param {style} style to write to byte[y][x] in buffer (ON, OFF, INVERT)
 * @param {val} value of byte
 * @returns {void}
 */
static void gui_write_byte(int16_t x, int16_t y ,draw_style style, uint8_t val)
{
	if( x > (GUI_CFG_DISPLAY_W - 1) || y > (GUI_CFG_DISPLAY_H/8 - 1) || x < 0 || y < 0) return;
	switch (style)
	{
	case DRAW_STATE_ON:
		gui_scr_buffer[(y  * GUI_CFG_DISPLAY_W) + x] |= val;
		break;
	case DRAW_STATE_OFF:
		gui_scr_buffer[(y * GUI_CFG_DISPLAY_W) + x] &= ~val;
		break;
	case DRAW_STATE_INVERT:
		gui_scr_buffer[(y  * GUI_CFG_DISPLAY_W) + x] ^= val;
		break;	
	default:
		break;
	}
}


/**
 * Description
 * @param {win} window area to draw
 * @param {x} x coordinate of byte
 * @param {y} y coordinate of byte
 * @param {style} style to write to byte[y][x] in window (ON, OFF, INVERT)
 * @param {val} value of byte
 * @returns {void}
 */
static void gui_window_write_byte(window * win ,int16_t x, int16_t y, draw_style style, uint8_t val)
{
    uint8_t n = 0,m = 0,temp1 = 0,temp2 = 0;
	if (x > win->w || y > win->h || x < 0 || y <= -7)
		return;
	if (y + 7 > (win->h - 1))
		val &= (~(0xFF << (win->h - y)));
	if(y > -7 && y < 0)
	{
		val >>=(-1*y);
		n = win->start_y/8;
		m = win->start_y%8;
	} 
	else
	{
    n = (win->start_y + y)/8; 
		m = (win->start_y + y)%8;
	}
	temp1 = val << m; 
	temp2 = (val >> (8-m)); 
    if(m==0) gui_write_byte(win->start_x+x,n,style,val); 
    else if(m != 0)
    {
        gui_write_byte(win->start_x+x,n,style,temp1);        
        gui_write_byte(win->start_x+x,n+1,style,temp2);        
    }
}























void gui_clear_buff(void)
{
  memset(gui_scr_buffer, 0, GUI_CFG_DISPLAY_W * GUI_CFG_DISPLAY_H / 8);
}
void gui_refresh(void)
{
   oled_update(gui_scr_buffer);
}

void gui_window_draw_pixel(window *win, int16_t x, int16_t y, draw_style style)
{
	gui_window_write_byte(win, x, y, style, 0x01);
}

void gui_window_draw_bmp(window * win, int16_t x, int16_t y, int16_t width, int16_t height,const uint8_t * bmp, draw_style style)
{
	if (bmp == NULL)
		return;
	uint8_t n = height / 8, m = height % 8;
	for (uint8_t i = 0; i < n; i++)
	{
		for (uint8_t j = 0; j < width; j++)
		{
			if (x + j > win->w)
				break;
			if (y + i * 8 > win->h)
				return;
			gui_window_write_byte(win, x + j, y + i * 8, style, (bmp[i * width + j]));
		}
	}
	if (m != 0)
	{
		for (uint8_t j = 0; j < width; j++)
		{
			if (x + j > win->w)
				return;
			gui_window_write_byte(win, x + j, y + n * 8, style, (bmp[n * width + j] & (~(0xFF << m))));
		}
	}
}
void gui_window_draw_line(window *win, int16_t x0, int16_t y0, int16_t x1, int16_t y1, draw_style style) 
{

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      gui_window_draw_pixel(win, y0, x0, style);
    } else {
      gui_window_draw_pixel(win, x0, y0, style);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void gui_window_draw_vline(window *win, int16_t x, int16_t y, int16_t h, draw_style style) 
{
  gui_window_draw_line(win, x, y, x, y + h - 1, style);
}
void gui_window_draw_hline(window *win, int16_t x, int16_t y, int16_t w, draw_style style) 
{
  gui_window_draw_line(win, x, y, x + w - 1, y, style);
}

void gui_window_draw_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, draw_style style) 
{
	gui_window_draw_hline(win, x, y, w - 1, style);
  	gui_window_draw_hline(win, x, y + h - 1, w - 1, style);
  	gui_window_draw_vline(win, x, y + 1, h - 2, style);
  	gui_window_draw_vline(win, x + w - 1, y, h, style);
}

void gui_window_fill_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, draw_style style) 
{
  for (int16_t i = x; i < x + w; i++) 
  {
    gui_window_draw_vline(win, i, y, h, style);
  }
}

void gui_window_draw_circle(window *win, int16_t x0, int16_t y0, int16_t r, draw_style style) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  gui_window_draw_pixel(win, x0, y0 + r, style);
  gui_window_draw_pixel(win, x0, y0 - r, style);
  gui_window_draw_pixel(win, x0 + r, y0, style);
  gui_window_draw_pixel(win, x0 - r, y0, style);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    gui_window_draw_pixel(win, x0 + x, y0 + y, style);
    gui_window_draw_pixel(win, x0 - x, y0 + y, style);
    gui_window_draw_pixel(win, x0 + x, y0 - y, style);
    gui_window_draw_pixel(win, x0 - x, y0 - y, style);
    gui_window_draw_pixel(win, x0 + y, y0 + x, style);
    gui_window_draw_pixel(win, x0 - y, y0 + x, style);
    gui_window_draw_pixel(win, x0 + y, y0 - x, style);
    gui_window_draw_pixel(win, x0 - y, y0 - x, style);
  }
}

/**
 * cornername	Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
 */
void gui_window_draw_circle_helper(window * win, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, draw_style style) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y - 1) {
    if (f > 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      gui_window_draw_pixel(win, x0 + x, y0 + y, style);
      gui_window_draw_pixel(win, x0 + y, y0 + x, style);
    }
    if (cornername & 0x2) {
      gui_window_draw_pixel(win, x0 + x, y0 - y, style);
      gui_window_draw_pixel(win, x0 + y, y0 - x, style);
    }
    if (cornername & 0x8) {
      gui_window_draw_pixel(win, x0 - y, y0 + x, style);
      gui_window_draw_pixel(win, x0 - x, y0 + y, style);
    }
    if (cornername & 0x1) {
      gui_window_draw_pixel(win, x0 - y, y0 - x, style);
      gui_window_draw_pixel(win, x0 - x, y0 - y, style);
    }
  }
	if (cornername & 0x4) {
		gui_window_draw_pixel(win, x0 + r, y0, style);
	}
	if (cornername & 0x2) {
		gui_window_draw_pixel(win, x0, y0 - r, style);
	}
	if (cornername & 0x8) {
		gui_window_draw_pixel(win, x0, y0 + r, style);
	}
	if (cornername & 0x1) {
		gui_window_draw_pixel(win, x0 - r, y0, style);
	}
}

void gui_window_fill_circle_helper(window *win, int16_t x0, int16_t y0, int16_t r,
                                    uint8_t corners, int16_t delta, draw_style style) 
{

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (corners & 0x1) {
      gui_window_draw_vline(win, x0+x, y0-y, 2*y+1+delta, style);
      gui_window_draw_vline(win, x0+y + 1, y0-x, 2*x+1+delta, style);
    }
    if (corners & 0x2) {
      gui_window_draw_vline(win, x0-x, y0-y, 2*y+1+delta, style);
      gui_window_draw_vline(win, x0-y - 1, y0-x, 2*x+1+delta, style);
    }
  }
}

void gui_window_draw_round_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, draw_style style) {
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  gui_window_draw_hline(win, x + r, y, w - 2 * r, style);         // Top
  gui_window_draw_hline(win, x + r, y + h - 1, w - 2 * r, style); // Bottom
  gui_window_draw_vline(win, x, y + r, h - 2 * r, style);         // Left
  gui_window_draw_vline(win, x + w - 1, y + r, h - 2 * r, style); // Right
  // draw four corners
  gui_window_draw_circle_helper(win, x + r, y + r, r, CORNER_0, style);
  gui_window_draw_circle_helper(win, x + w - r - 1, y + r, r, CORNER_1, style);
  gui_window_draw_circle_helper(win, x + w - r - 1, y + h - r - 1, r, CORNER_2, style);
  gui_window_draw_circle_helper(win, x + r, y + h - r - 1, r, CORNER_3, style);
}

void gui_window_fill_round_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, draw_style style) {

  gui_window_fill_rect(win, x+r, y, w-2*r, h, style);

  // draw four corners
  gui_window_fill_circle_helper(win, x+w-r-1, y+r, r, CORNER_0, h-2*r-1, style);
  gui_window_fill_circle_helper(win, x+r    , y+r, r, CORNER_1, h-2*r-1, style);
}


void gui_window_draw_char(window *win, int16_t x, int16_t y , char c, draw_style style)
{
	uint16_t index =  c - ' ';
  for(uint8_t i=0;i<6;i++)
  {
    gui_window_write_byte(win,x,y,DRAW_STATE_ON, gui_font_6x8[index][i]);
    x++;
    if(x > win->w)break; 
  }

}

void gui_window_draw_str(window *win,int16_t x, int16_t y , const char* str, draw_style style)
{
	int16_t cur_x = x; 
  int16_t cur_y = y; 
	while(*str != '\0')
	{
    if(*str == '\n')
    {
      cur_y += 8;
    }
    else if(*str == '\r')
    {
      cur_x = x;
    }
    else
    {
      gui_window_draw_char(win, cur_x, cur_y, (uint32_t)*str, style);
      cur_x += 6;
    }
		str++;
		if(cur_x > win->w) break; 
	}
}