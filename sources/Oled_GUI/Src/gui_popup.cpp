#include "gui_popup.h"


void gui_popup::create(int16_t w, int16_t h)
{
    obj_backgnd.deinit();
	obj_backgnd.set_window(&window_screen_size);
    obj_backgnd.set_x(0);
    obj_backgnd.set_y(0);
    obj_backgnd.set_w(GUI_CFG_DISPLAY_W);
    obj_backgnd.set_h(GUI_CFG_DISPLAY_H);
    obj_backgnd.set_fade(5);
    obj_backgnd.add_state(GUI_STATE_DISABLE);



    this->deinit();
    this->set_window(&window_screen_size);
    this->set_x(0);
    this->set_y(0);
    this->set_w(0);
    this->set_h(0);

    this->x_popup = (GUI_CFG_DISPLAY_W - w) / 2;
    this->y_popup = (GUI_CFG_DISPLAY_H - h) / 2;
    this->w_popup = w;
    this->h_popup = h;
    this->is_showed = false;

    label.create(x_popup + 5, y_popup + 5);
    label.add_state(GUI_STATE_DISABLE);
    gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(&label));
    
    gui_obj::gui_obj_add_tail(&obj_backgnd);
	gui_obj::gui_obj_add_tail(dynamic_cast<gui_obj*>(this));
    gui_obj::gui_obj_add_tail(dynamic_cast<gui_obj*>(&label));
}

void gui_popup::show()
{
    if(!this->is_showed)
    {
        this->is_showed = true;
        obj_backgnd.clear_state(GUI_STATE_DISABLE);
        label.clear_state(GUI_STATE_DISABLE);
        this->set_x(0);
        this->set_y(0);
        this->set_w(0);
        this->set_h(0);
        anim.create_resize(this, x_popup, y_popup, w_popup, h_popup, 300);
    }

}
void gui_popup::hide()
{
    if(this->is_showed)
    {
        this->is_showed = false;
        obj_backgnd.add_state(GUI_STATE_DISABLE);
        label.add_state(GUI_STATE_DISABLE);
        this->set_x(x_popup);
        this->set_y(y_popup);
        this->set_w(w_popup);
        this->set_h(h_popup);
        anim.create_resize(this, 0, 0, 0, 0, 300);
    }
}

void gui_popup::toggle()
{
    if(this->is_showed) hide();
    else show();
}


void gui_popup::draw()
{   
    if(this->get_x() != 0)
    {
        gui_window_fill_rect(this->get_window(), this->get_x(), this->get_y(), this->get_w(), this->get_h(), DRAW_STATE_OFF);
        gui_window_draw_rect(this->get_window(), this->get_x(), this->get_y(), this->get_w(), this->get_h(), DRAW_STATE_ON);
    }
}