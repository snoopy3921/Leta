#include "gui_group.h"
#include "gui_port.h"

void gui_group::create(int16_t x, int16_t y, int16_t w, int16_t h)
{
    this->deinit();
    group_win.start_x = x;
    group_win.start_y = y;
    group_win.w = w;
    group_win.h = h;
    this->gui_obj::set_window(&group_win);
    this->gui_obj::set_x(x);
    this->gui_obj::set_y(y);
    this->gui_obj::set_w(w);
    this->gui_obj::set_h(h);
    mem_sum = 0;
    for(uint8_t i = 0; i < MAX_OBJ_MEMBER; i++)
    {
        member[i] = GUI_OBJ_NULL;
    }
    focus_index = -1;
    // gui_obj::gui_obj_add_head(dynamic_cast<gui_obj*>(this));
}

void gui_group::set_x(int16_t x)
{
    // if(idctor.get_target() != GUI_OBJ_NULL) 
    // {
    //     idctor.set_x(idctor.get_x()+ (x - this->get_x()));
    // }
    for(uint8_t i = 0; i < mem_sum; i++)
    {
        member[i]->set_x(member[i]->get_x() - this->get_x() + x);
    }
    gui_obj::set_x(x);
}

void gui_group::set_y(int16_t y)
{
    // if(idctor.get_target() != GUI_OBJ_NULL) 
    // {
    //     idctor.set_y(idctor.get_y()+ (y - this->get_y()));
    // }   
    for(uint8_t i = 0; i < mem_sum; i++)
    {
        member[i]->set_y(member[i]->get_y() - this->get_y() + y);
    }
    gui_obj::set_y(y); 
}

void gui_group::set_fade(uint8_t fade)
{
	if(idctor.get_target() != GUI_OBJ_NULL) idctor.set_fade(fade);
    for(uint8_t i = 0; i < mem_sum; i++)
    {
        member[i]->set_fade(fade);
    }
    gui_obj::set_fade(fade);
}

void gui_group::set_pos_offset(int16_t delta_x, int16_t delta_y)
{
    if(idctor.get_target() != GUI_OBJ_NULL) idctor.set_x(idctor.get_x()+delta_x);
    if(idctor.get_target() != GUI_OBJ_NULL) idctor.set_y(idctor.get_y()+delta_y);
    for(uint8_t i = 0; i < mem_sum; i++)
    {
        member[i]->set_x(member[i]->get_x()+delta_x);
        member[i]->set_y(member[i]->get_y()+delta_y);
    }
}

void gui_group::add_obj(gui_obj* obj, gui_flag is_obj_focusable)
{
    if(obj == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to add null target");
        return;
    }
    if(is_obj_focusable == GUI_FLAG_FOCUSABLE) 
    {
        obj->add_flag(GUI_FLAG_FOCUSABLE);
        if(focus_index == -1)
        {            
            idctor.create(obj, 2);
            idctor.set_y(idctor.get_y() - this->get_y());
            idctor.set_window(&group_win);
            focus_index = 0;
        }
        member[mem_sum] = obj;
        mem_sum++;
    }
    else
    {
        member[mem_sum] = obj;
        mem_sum++;
    }
    obj->set_y(obj->get_y() - this->get_y());
    obj->set_window(&group_win);
}

gui_obj* gui_group::get_focus_obj()
{
	return member[focus_index];
}

int8_t gui_group::get_focus_index()
{
	return focus_index;
}

static void idctor_focus_member(void *p_target)
{
    gui_group *temp_group = (gui_group *)p_target;
    temp_group->get_idctor()->set_target(temp_group->get_focus_obj());
}


void gui_group::focus_obj(uint8_t direction)
{
    if(mem_sum == 0 || focus_index == -1) 
    {
        GUI_LOG("Group empty or no no focusable obj");
        return;
    }
    int8_t index = focus_index;
    uint8_t gap = idctor.get_gap();
    switch (direction)
    {
    case FOCUS_NEXT:
        do
        {
            index++;
            if(index == mem_sum) index = 0;
        }while(!member[index]->check_flag(GUI_FLAG_FOCUSABLE));

        if(index != focus_index ) focus_index = index;

        break;
    case FOCUS_PREV:
        do
        {
            index--;
            if(index == -1) index = mem_sum - 1;
        }while(!member[index]->check_flag(GUI_FLAG_FOCUSABLE));

        if(index != focus_index ) focus_index = index;

        break;    
    default:
        break;
    }
    
    int16_t bias_x = 0;
    int16_t bias_y = 0;
    int16_t min_x = member[focus_index]->get_x() - gap;
    int16_t min_y = member[focus_index]->get_y() - gap;
    int16_t max_x = member[focus_index]->get_x() + member[focus_index]->get_w() + gap;
    int16_t max_y = member[focus_index]->get_y() + member[focus_index]->get_h() + gap;

	if( min_x < 0)
		bias_x = 0 - min_x;
	else if( max_x > group_win.w )
		bias_x = group_win.w - max_x;

	if( min_y < 0)
		bias_y = 0 - min_y ;
	else if( max_y > group_win.h )
		bias_y = group_win.h - max_y ;

	if(bias_x != 0 || bias_y != 0) 
    {
        anim1.create_move_offset(this, bias_x , bias_y, 200);
    }
    anim.create_resize(&idctor, member[focus_index]->get_x() - gap + bias_x, member[focus_index]->get_y() - gap + bias_y, member[focus_index]->get_w() + 2 * gap, member[focus_index]->get_h() + 2 * gap, 300);
    anim.set_anim_end_cb(idctor_focus_member, this);
    return;
}


/* Custom draw something for group*/
// void gui_group::draw()
// {
// 	gui_window_draw_rect(&window_screen_size, group_win.start_x, group_win.start_y, group_win.w, group_win.h, DRAW_STATE_ON);
// }