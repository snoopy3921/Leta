#include "gui_label.h"
#include <string.h>
#include <stdarg.h>
#include <cstdio>
#include "xprintf.h"


void gui_label::create(int16_t x, int16_t y)
{
    this->deinit();
	this->set_window(&window_screen_size);
    this->set_x(x);
    this->set_y(y);
    this->set_w(0);
    this->set_h(0);
	text[0] = '\0';
	gui_obj::gui_obj_add_head(dynamic_cast<gui_obj*>(this));
}

void gui_label::printf(const char *str, ...)
{
    va_list args;
	va_start(args, str);
    xvfprintf_buff(text, str, args);
    va_end(args);

    char* p_char = text;
    int16_t cur_x = this->get_x(); 
    int16_t cur_y = this->get_y(); 
    int16_t max_x = cur_x; 
    int16_t max_y = cur_y; 
	while(*p_char != '\0')
	{
        if(*p_char == '\n')
        {
            cur_y += 8;
        }
        else if(*p_char == '\r')
        {
            cur_x = this->get_x();
        }
        else
        {
            cur_x += 6;
        }
        max_x = (cur_x > max_x)? cur_x : max_x;
        max_y = (cur_y > max_y)? cur_y : max_y;
        p_char++;
	}
    this->set_w(max_x - this->get_x());
    this->set_h(max_y + 8 - this->get_y());
}

void gui_label::clear()
{
	this->set_w(0);
	text[0] = '\0';
}

void gui_label::draw()
{
    gui_window_draw_str(this->get_window(), this->get_x(), this->get_y(), this->text, DRAW_STATE_ON);	
}