#include "gui_idctor.h"
#include "gui_port.h"


void gui_idctor::create(gui_obj* target, uint8_t gap)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to indicate null target");
        return;
    }
    this->deinit();
	this->set_window(&window_screen_size);
    this->target = target;
    this->set_x(target->get_x() - gap);
    this->set_y(target->get_y() - gap);
    this->set_w(target->get_w() + 2*gap);
    this->set_h(target->get_h() + 2*gap);    
    this->set_gap(gap);
	gui_obj::gui_obj_add_tail(dynamic_cast<gui_obj*>(this));
}

void gui_idctor::set_target(gui_obj* target)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to indicate null target");
        return;
    }  
    this->target = target;
    this->set_x(target->get_x() - gap);
    this->set_y(target->get_y() - gap);
    this->set_w(target->get_w() + 2*gap);
    this->set_h(target->get_h() + 2*gap);  
}
void gui_idctor::draw()
{
    gui_window_fill_round_rect(this->get_window(), this->get_x(), this->get_y(), this->get_w(), this->get_h(), 3, DRAW_STATE_INVERT);
}