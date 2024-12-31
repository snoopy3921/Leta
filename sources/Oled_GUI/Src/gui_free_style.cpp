#include "gui_free_style.h"

void gui_free_style::create(int16_t x, int16_t y, int16_t w, int16_t h, custom_draw custom_draw_func)
{
    this->deinit();
	this->set_window(&window_screen_size);
    this->set_x(x);
    this->set_y(y);
    this->set_w(w);
    this->set_h(h);
    this->custom_draw_func = custom_draw_func;
	gui_obj::gui_obj_add_tail(dynamic_cast<gui_obj*>(this));
}

void gui_free_style::draw()
{
    this->custom_draw_func(this);
}