
#ifndef __GUI_RENDER_H__
#define __GUI_RENDER_H__
/**
 * This file provide API to render BUFFER given by display
 */
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_cfg.h"

typedef struct 
{   
    int16_t start_x;
    int16_t start_y;
    int16_t w;
    int16_t h;
} window;

typedef enum 
{   
    DRAW_STATE_ON = 0,
    DRAW_STATE_OFF,
    DRAW_STATE_INVERT
} draw_style;

enum 
{   
    CORNER_0 = 0x01,
    CORNER_1 = 0x02,
    CORNER_2 = 0x04,
    CORNER_3 = 0x08,
};

extern uint8_t gui_scr_buffer[GUI_CFG_DISPLAY_W * GUI_CFG_DISPLAY_H / 8];


extern void gui_clear_buff(void);
extern void gui_refresh(void);

extern void gui_window_draw_pixel(window *win, int16_t x, int16_t y, draw_style style);
extern void gui_window_draw_hline(window *win, int16_t x, int16_t y, int16_t w, draw_style style);
extern void gui_window_draw_vline(window *win, int16_t x, int16_t y, int16_t h, draw_style style);
extern void gui_window_draw_line(window *win, int16_t x0, int16_t y0, int16_t x1, int16_t y1, draw_style style);
extern void gui_window_draw_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, draw_style style);
extern void gui_window_fill_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, draw_style style); 
extern void gui_window_draw_circle(window *win, int16_t x0, int16_t y0, int16_t r, draw_style style);
extern void gui_window_draw_circle_helper(window * win, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, draw_style style);
extern void gui_window_fill_circle_helper(window *win, int16_t x0, int16_t y0, int16_t r,
                                    uint8_t corners, int16_t delta, draw_style style) ;
extern void gui_window_draw_round_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, draw_style style);
extern void gui_window_fill_round_rect(window *win, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, draw_style style);
extern void gui_window_draw_bmp(window * win, int16_t x, int16_t y, int16_t width, int16_t height,const uint8_t * bmp, draw_style style);
extern void gui_window_draw_char(window *win,int16_t x, int16_t y , char c, draw_style style);
extern void gui_window_draw_str(window *win,int16_t x, int16_t y ,const char* str, draw_style style);

#ifdef __cplusplus
}
#endif
#endif /* __GUI_RENDER_H__ */
