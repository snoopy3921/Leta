#include "gui_check_box.h"

void gui_chkbox::create(int16_t x, int16_t y, uint8_t size, gui_state state)
{
    this->deinit();
	this->set_window(&window_screen_size);
    this->add_state(state);
    this->set_x(x);
    this->set_y(y);
    switch (size)
    {
    case CHKBOX_SIZE_1:
        /* code */
		this->set_w(8);
		this->set_h(8);
        break;
    case CHKBOX_SIZE_2:
        /* code */
        this->set_w(16);
		this->set_h(16);
        break;
    }
	gui_obj::gui_obj_add_head(dynamic_cast<gui_obj*>(this));
}
void gui_chkbox::toggle()
{
    if(this->check_state(GUI_STATE_CHECKED)) this->clear_state(GUI_STATE_CHECKED);
    else this->add_state(GUI_STATE_CHECKED);
}
void gui_chkbox::draw()
{
	gui_window_draw_rect(this->get_window(), this->get_x(), this->get_y(), this->get_w(), this->get_h(), DRAW_STATE_ON);

    if(this->check_state(GUI_STATE_CHECKED))
    {
        gui_window_fill_rect(this->get_window(), this->get_x() + 2, this->get_y() + 2, this->get_w() - 4, this->get_h() - 4, DRAW_STATE_ON);
    }
}