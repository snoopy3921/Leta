#include "gui_core.h"
#include "gui_port.h"
window window_screen_size;


static uint32_t gui_tick = 0;
static uint32_t sum = 0;

uint32_t gui_get_tick_now(void)
{
    return get_tick();
}
uint32_t gui_get_tick_elap(uint32_t prev_tick)
{
    uint32_t const tick_now = get_tick();
    uint32_t tick_elap = 0;
    if (tick_now >= prev_tick)
    {
        tick_elap = tick_now - prev_tick;
    }
    else /* Overflown */
    {
        tick_elap = GUI_CFG_TICK_MAX - prev_tick + tick_now;
    }
    return tick_elap;
}

gui_obj * gui_obj::obj_head_ptr;
gui_obj * gui_obj::obj_tail_ptr;
void gui_obj::gui_init(/* args */)
{
    gui_obj::gui_obj_init();
    window_screen_size.start_x = 0;
    window_screen_size.start_y = 0;
    window_screen_size.w = GUI_CFG_DISPLAY_W;
    window_screen_size.h = GUI_CFG_DISPLAY_H;

    /**
     * TODO: 
     * Add anim init,..
     */
}

void gui_obj::gui_obj_init(/* args */)
{
    obj_head_ptr = GUI_OBJ_NULL;
    obj_tail_ptr = GUI_OBJ_NULL;
}
void gui_obj::gui_obj_add_head(gui_obj * obj)
{
    if(obj_head_ptr == GUI_OBJ_NULL)
    {
        obj->next_obj_ptr = GUI_OBJ_NULL;
        obj->prev_obj_ptr = GUI_OBJ_NULL;
        obj_head_ptr = obj;
        obj_tail_ptr = obj;
    }
    else
    {
        obj->next_obj_ptr = obj_head_ptr;
        obj->prev_obj_ptr = GUI_OBJ_NULL;
        obj_head_ptr->prev_obj_ptr = obj;
        obj_head_ptr = obj;
    }        
    sum++;
}
void gui_obj::gui_obj_add_tail(gui_obj * obj)
{
    if(obj_head_ptr == GUI_OBJ_NULL)
    {
        gui_obj_add_head(obj);
    }
    else
    {
        obj->next_obj_ptr = GUI_OBJ_NULL;
        obj->prev_obj_ptr = obj_tail_ptr;
        obj_tail_ptr->next_obj_ptr = obj;
        obj_tail_ptr = obj;

        sum++;
    }
}

void gui_obj::gui_obj_remove(gui_obj * obj)
{
    if(obj == GUI_OBJ_NULL) return;
    if (obj == obj_head_ptr)
	{
		if (obj->next_obj_ptr != GUI_OBJ_NULL)
		{
			obj->next_obj_ptr->prev_obj_ptr = GUI_OBJ_NULL;
			obj_head_ptr = obj->next_obj_ptr;
		}
		else
		{
			obj_head_ptr = GUI_OBJ_NULL;
			obj_tail_ptr = GUI_OBJ_NULL;
		}
	}
	else if (obj == obj_tail_ptr)
	{
		obj->prev_obj_ptr->next_obj_ptr = GUI_OBJ_NULL;
		obj_tail_ptr = obj->prev_obj_ptr;
	}
	else
	{
		obj->prev_obj_ptr->next_obj_ptr = obj->next_obj_ptr;
		obj->next_obj_ptr->prev_obj_ptr = obj->prev_obj_ptr;
	}
    sum--;
}


void gui_obj::gui_obj_render()
{
	gui_obj *p_obj = obj_head_ptr;
    if(p_obj == GUI_OBJ_NULL) return;
    gui_clear_buff();
	while (p_obj != GUI_OBJ_NULL)
	{
		if (p_obj->check_state(GUI_STATE_DISABLE))
			goto next;
        // if (!p_obj->check_state(GUI_STATE_CHANGED))
		// 	goto next;
		if ((p_obj->x > GUI_CFG_DISPLAY_W) || (p_obj->x + p_obj->w) < 0)
			goto next;
		if ((p_obj->y > GUI_CFG_DISPLAY_H) || (p_obj->y + p_obj->h) < 0)
			goto next;
		p_obj->draw();
		p_obj->draw_fade();
	next:
		p_obj = p_obj->next_obj_ptr;
	}
	gui_refresh();
}

void gui_obj::set_allign(gui_align allign)
{
	switch (allign)
	{
	case GUI_ALIGN_TOP:
		this->y = 0;
		break;
	case GUI_ALIGN_BOTTOM:
		this->y = this->window_ptr->start_y + this->window_ptr->h - this->h;
		break;
	case GUI_ALIGN_LEFT:
		this->x = 0;
		break;
	case GUI_ALIGN_RIGHT:
		this->x = this->window_ptr->start_x + this->window_ptr->w - this->w;
		break;
	case GUI_ALIGN_HOR_MID:
		this->x = (this->window_ptr->start_x + this->window_ptr->w - this->w) / 2;
		break;
	case GUI_ALIGN_VER_MID:
		this->y = (this->window_ptr->start_y + this->window_ptr->h - this->h) / 2;
		break;
	case GUI_ALIGN_TOP_LEFT:
		this->x = 0;
		this->y = 0;
		break;
	case GUI_ALIGN_TOP_MID:
		this->x = (this->window_ptr->start_x + this->window_ptr->w - this->w) / 2;
		this->y = 0;
		break;
	case GUI_ALIGN_TOP_RIGHT:
		this->x = this->window_ptr->start_x + this->window_ptr->w - this->w;
		this->y = 0;
		break;
	case GUI_ALIGN_BOTTOP_LEFT:
		this->x = 0;
		this->y = this->window_ptr->start_y + this->window_ptr->h - this->h;
		break;
	case GUI_ALIGN_BOTTOP_MID:
		this->x = (this->window_ptr->start_x + this->window_ptr->w - this->w) / 2;
		this->y = this->window_ptr->start_y + this->window_ptr->h - this->h;
		break;
	case GUI_ALIGN_BOTTOP_RIGHT:
		this->x = this->window_ptr->start_x + this->window_ptr->w - this->w;
		this->y = this->window_ptr->start_y + this->window_ptr->h - this->h;
		break;
	}
}


/** 
 *  Schema fading from 1 - 9 for every 9-dot
 *  1 8 3
 *  7 5 6
 *  4 9 2 
 */
void gui_obj::draw_fade(void)
{
    uint8_t fade_step = 3;
    uint8_t fade_temp = fade;
    while(fade_temp)
    {
        switch (fade_temp)
        {
        case 9:
            for (int16_t i = x + 1; i < x + w; i += fade_step)
            {
                for (int16_t j = y + 2; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;
        case 8:
            for (int16_t i = x + 1; i < x + w; i += fade_step)
            {
                for (int16_t j = y; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;
        case 7:
            for (int16_t i = x; i < x + w; i += fade_step)
            {
                for (int16_t j = y + 1; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;  
        case 6:
            for (int16_t i = x + 2; i < x + w; i += fade_step)
            {
                for (int16_t j = y + 1; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;    
        case 5:
            for (int16_t i = x + 1; i < x + w; i += fade_step)
            {
                for (int16_t j = y + 1; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;   
        case 4:
            for (int16_t i = x; i < x + w; i += fade_step)
            {
                for (int16_t j = y + 2; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;  
        case 3:
            for (int16_t i = x + 2; i < x + w; i += fade_step)
            {
                for (int16_t j = y; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;  
        case 2:
            for (int16_t i = x + 2; i < x + w; i += fade_step)
            {
                for (int16_t j = y + 2; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break; 
        case 1:
            for (int16_t i = x; i < x + w; i += fade_step)
            {
                for (int16_t j = y; j < y + h; j += fade_step)
                {
                    gui_window_draw_pixel(&window_screen_size, i, j, DRAW_STATE_OFF);
                }
            }
            break;       
        default:
            break;
        }
        fade_temp--;
    }
    
}
void gui_obj::draw()
{
    /**
     * Derived obj draw need to be implemented here(button, switch,...)
     * TODO:
    */
    
}