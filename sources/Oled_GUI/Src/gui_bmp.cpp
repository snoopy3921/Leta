#include "gui_bmp.h"


void gui_bmp::create(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *p_bmp)
{
    this->deinit();
	this->set_window(&window_screen_size);
    this->set_x(x);
    this->set_y(y);
    this->set_w(w);
    this->set_h(h);
    this->set_feature((void *)p_bmp);
	gui_obj::gui_obj_add_head(dynamic_cast<gui_obj*>(this));
}

void gui_bmp:: draw()
{
    gui_window_draw_bmp(this->get_window(), this->get_x(), this->get_y(), this->get_w(), this->get_h(), (uint8_t *)this->get_feature(), DRAW_STATE_ON);
}