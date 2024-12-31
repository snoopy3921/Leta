#include "gui_slider.h"
#include "gui_port.h"

void gui_slider::create(int16_t x, int16_t y, int16_t len, int16_t min, int16_t max, int16_t start_value, slider_type type)
{
    if(start_value < min || start_value > max)
    {
        GUI_LOG("Invalid start value");
        return;
    }
    if(min >= max)
    {
        GUI_LOG("Invalid min max");
        return;
    }
    this->deinit();
    this->set_window(&window_screen_size);
    this->set_x(x);
    this->set_y(y);
    switch (type)
    {
    case SLIDER_HOR:
        /* code */
        this->set_w(len);
        this->set_h(2);
        break;
    case SLIDER_VER:
        /* code */
        this->set_w(2);
        this->set_h(len);
        break;   
    default:
        break;
    }
    this->min = min;
    this->max = max;
    this->value = start_value;
    this->type = type;
    gui_obj::gui_obj_add_head(dynamic_cast<gui_obj*>(this));
}
void gui_slider::set_value(int16_t val)
{         
    if(val < this->min) this->value = this->min;
    else if(val > this->max) this->value = this->max;
    else this->value = val;
}
void gui_slider::draw()
{
    gui_window_fill_rect(this->get_window(), this->get_x(), this->get_y(), this->get_w(), this->get_h(), DRAW_STATE_ON);
    float percent;
    switch (this->type)
    {
    case SLIDER_HOR:
        /* code */
        percent = (this->value - this->min)  / (float)(this->max - this->min) * (float)(this->get_w());
        gui_window_fill_rect(this->get_window(), this->get_x() + percent - 5, this->get_y() - 2, 10, 6, DRAW_STATE_INVERT);
        break;
    case SLIDER_VER:
        percent = (this->value - this->min)  / (float)(this->max - this->min) * (float)(this->get_h());
        gui_window_fill_rect(this->get_window(), this->get_x() - 2, this->get_y() + percent - 5, 6, 10, DRAW_STATE_INVERT);
        /* code */
        break;   
    default:
        break;
    }
}