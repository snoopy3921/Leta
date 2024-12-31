#include "gui_anim.h"
#include "math.h"
#include "gui_port.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

static uint16_t anim_timestamp = 0;

gui_anim *gui_anim::pFirst;
gui_anim *gui_anim::pLast;

static float animate_fx(float t) // function for animation
{
	//return t*t; /* Parabol */
    return t; /* Linear */
}

gui_anim::~gui_anim()
{
	destroy();
}

gui_anim::gui_anim()
{

}
void gui_anim::create_move(gui_obj* target, int16_t x_stop, int16_t y_stop, uint16_t duration)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to animate null target");
        return;
    }
    if(duration == 0)
    {
        GUI_LOG("Duration can not be zero");
        return;
    }
    if(this->target != GUI_OBJ_NULL)
    {
        this->stop();
    }
    this->target = target;
    this->x_start = target->get_x();
    this->y_start = target->get_y();
    this->x_stop = x_stop;
    this->y_stop = y_stop;
    this->duration = duration;
    this->act_time = 0;
    this->w_start = -1;
    this->fade_start = -1;
    this->anim_end_cb = GUI_ANIM_NULL_CB;
    this->arg_ptr = nullptr;
    this->swap_cnt = 0;
    this->state = ANIM_READY;
    gui_anim::gui_anim_add(this);
}
void gui_anim::create_resize(gui_obj* target, int16_t x_stop, int16_t y_stop, int16_t w_stop, int16_t h_stop, uint16_t duration)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to animate null target");
        return;
    }
    if(duration == 0)
    {
        GUI_LOG("Duration can not be zero");
        return;
    }
    if(this->target != GUI_OBJ_NULL)
    {
        this->stop();
    }
    this->target = target;
    this->x_start = target->get_x();
    this->y_start = target->get_y();
    this->w_start = target->get_w();
    this->h_start = target->get_h();
    this->x_stop = x_stop;
    this->y_stop = y_stop;
    this->w_stop = w_stop;
    this->h_stop = h_stop;
    this->duration = duration;
    this->act_time = 0;
    this->fade_start = -1;
    this->anim_end_cb = GUI_ANIM_NULL_CB;
    this->arg_ptr = nullptr;
    this->swap_cnt = 0;
    this->state = ANIM_READY;
    gui_anim::gui_anim_add(this);
}
void gui_anim::create_fade(gui_obj* target, uint8_t fade_start, uint8_t fade_stop, uint16_t duration)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to animate null target");
        return;
    }
    if(duration == 0)
    {
        GUI_LOG("Duration can not be zero");
        return;
    }
    if(this->target != GUI_OBJ_NULL)
    {
        this->stop();
    }
    this->target = target;
    this->fade_start = target->get_fade();
    this->duration = duration;
    this->act_time = 0;
    this->fade_start = fade_start;
    this->fade_stop = fade_stop;
    this->x_start = -1;
    this->w_start = -1;
    this->anim_end_cb = GUI_ANIM_NULL_CB;
    this->arg_ptr = nullptr;
    this->swap_cnt = 0;
    this->state = ANIM_READY;
    gui_anim::gui_anim_add(this);
}

void gui_anim::create_move_offset(gui_obj* target, int16_t delta_x, int16_t delta_y, uint16_t duration)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to animate null target");
        return;
    }
    int16_t x_stop = target->get_x() + delta_x;
    int16_t y_stop = target->get_y() + delta_y;
    this->create_move(target, x_stop, y_stop, duration);
}
void gui_anim::create_resize_offset(gui_obj* target, int16_t delta_x, int16_t delta_y, int16_t delta_w, int16_t delta_h, uint16_t duration)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to animate null target");
        return;
    }
    int16_t x_stop = target->get_x() + delta_x;
    int16_t y_stop = target->get_y() + delta_y;
    int16_t w_stop = target->get_w() + delta_w;
    int16_t h_stop = target->get_h() + delta_h;
    this->create_resize(target, x_stop, y_stop, w_stop, h_stop, duration);   
}
void gui_anim::create_fade_offset(gui_obj* target, uint8_t delta_fade, uint16_t duration)
{
    if(target == GUI_OBJ_NULL)
    {
        GUI_LOG("Try to animate null target");
        return;
    }
    int16_t fade_start = target->get_fade();
    int16_t fade_stop = fade_start + delta_fade;
    this->create_fade(target, fade_start, fade_stop, duration);   
}

void gui_anim::set_behavior(uint16_t playback_duration, bool is_infinity)
{
    this->playback_duration = playback_duration;
    this->is_infinity = is_infinity;
}
void gui_anim::stop()
{
    if(this->state == ANIM_READY)
    {
        GUI_LOG("Try to stop target not animating");
        return;
    }
    gui_anim::gui_anim_delete(this);

    // /* Return start state */
    // if(this->swap_cnt == 1)
    // {
    //     gui_anim_inverse_attribute(this);
    // }
    // if(this->x_start != -1) 
    // {
    //     this->target->set_x(this->x_start);
    //     this->target->set_y(this->y_start);
    // }
    // if(this->w_start != -1) 
    // {
    //     this->target->set_w(this->w_start);
    //     this->target->set_h(this->h_start);
    // }
    // if(this->fade_start != -1)
    // {
    //     this->target->set_fade(this->fade_start);
    // }
}



void gui_anim::gui_anim_init(void)
{
	pFirst = GUI_ANIM_NULL;
	pLast = GUI_ANIM_NULL;
}
void gui_anim::gui_anim_add(gui_anim *pAnim)
{
	if(pAnim == GUI_ANIM_NULL)    
    {
        GUI_LOG("Try to add null animation");
        return;
    }
    if(pAnim->state == ANIM_RUNNING)
    {
        GUI_LOG("Try to add target has been animating");
        return;
    }
    if(pFirst == GUI_ANIM_NULL)
    {
        pAnim->pNext = GUI_ANIM_NULL;
        pAnim->pPrev = GUI_ANIM_NULL;
        pFirst = pAnim;
		pLast = pAnim;
    }
	else
	{
		pAnim->pPrev = pLast;
		pAnim->pNext = GUI_ANIM_NULL;
		pLast->pNext = pAnim;
		pLast = pAnim;
	}
    pAnim->state = ANIM_RUNNING;
}
void gui_anim::gui_anim_delete(gui_anim *pAnim)
{
    if(pAnim == GUI_ANIM_NULL)
    {
        GUI_LOG("Try to delete null animation");
        return;
    }
    if(pAnim->state == ANIM_READY)
    {
        GUI_LOG("Try to delete non-active animation");
        return;
    }
	if (pAnim == pFirst)
	{
		if (pAnim->pNext != GUI_ANIM_NULL)
		{
			pAnim->pNext->pPrev = GUI_ANIM_NULL;
			pFirst = pAnim->pNext;
		}
		else
		{
			pFirst = GUI_ANIM_NULL;
			pLast = GUI_ANIM_NULL;
		}
	}
	else if (pAnim == pLast)
	{
        pAnim->pPrev->pNext = GUI_ANIM_NULL;
		pLast = pAnim->pPrev;
	}
	else
	{
        pAnim->pPrev->pNext = pAnim->pNext;
        pAnim->pNext->pPrev = pAnim->pPrev;
	}
    pAnim->state = ANIM_READY;
    pAnim->target = GUI_OBJ_NULL;
}

void gui_anim::gui_anim_inverse_attribute(gui_anim *pAnim)
{
    if(pAnim == GUI_ANIM_NULL) return;
    if(pAnim->x_start != -1) 
    {
        _swap_int16_t(pAnim->x_start, pAnim->x_stop);
        _swap_int16_t(pAnim->y_start, pAnim->y_stop);
    }
    if(pAnim->w_start != -1) 
    {
        _swap_int16_t(pAnim->w_start, pAnim->w_stop);
        _swap_int16_t(pAnim->h_start, pAnim->h_stop);
    }
    if(pAnim->fade_start != -1)
    {
        _swap_int16_t(pAnim->fade_start, pAnim->fade_stop);
    }
}
static float fps;
float gui_anim::get_fps(){ return fps; }
void gui_anim::gui_anim_run(void)
{
	gui_anim *pAnim = pFirst;
	uint16_t elaps = gui_get_tick_elap(anim_timestamp);
	anim_timestamp = gui_get_tick_now();
    

	while (pAnim != GUI_ANIM_NULL)
	{	
        
        bool anim_stop = false;
		volatile float p;
        int16_t x_dis = 0;
        int16_t y_dis = 0;
        int16_t w_dis = 0;
        int16_t h_dis = 0;
        int8_t fade_dis = 0;
        if(pAnim->x_start != -1)
        {
            x_dis = pAnim->x_stop - pAnim->x_start;
            y_dis = pAnim->y_stop - pAnim->y_start;
        }
        if(pAnim->w_start != -1)
        {
            w_dis = pAnim->w_stop - pAnim->w_start;
            h_dis = pAnim->h_stop - pAnim->h_start;
        }
        if(pAnim->fade_start != -1)
        {
            fade_dis = pAnim->fade_stop - pAnim->fade_start;
        }

		pAnim->act_time += elaps;


        if(pAnim->swap_cnt == 0)
        {
            fps = (float)1000/elaps;
            p = animate_fx((float)pAnim->act_time / pAnim->duration);
            if (pAnim->act_time > pAnim->duration) 
            {   
                
                if(pAnim->is_infinity == false)
                {
                    anim_stop = true;
                    p = 1;
                }  
                else
                {
                    pAnim->act_time = 0;
                    if(pAnim->playback_duration != 0)
                    {
                        pAnim->swap_cnt = !pAnim->swap_cnt;
                        gui_anim_inverse_attribute(pAnim);
                    }
                }
            }

        }
        else
        {
            fps = (float)1000/elaps;
            p = animate_fx((float)pAnim->act_time / pAnim->playback_duration);
            if (pAnim->act_time > pAnim->playback_duration) 
            {   
                if(pAnim->is_infinity == false)
                {
                    anim_stop = true;
                    p = 1;
                }  
                else
                {
                    pAnim->act_time = 0;
                    pAnim->swap_cnt = !pAnim->swap_cnt;
                    gui_anim_inverse_attribute(pAnim);
                }
            }
        }

		if (x_dis != 0)
		{
			pAnim->target->set_x(pAnim->x_start + x_dis * p);
		}
        if (y_dis != 0)
		{
			pAnim->target->set_y(pAnim->y_start + y_dis * p);
		}
		if (w_dis != 0)
		{
			pAnim->target->set_w(pAnim->w_start + w_dis * p);
		}
		if (h_dis != 0)
		{
			pAnim->target->set_h(pAnim->h_start + h_dis * p);
		}

		if (fade_dis != 0)
		{
            pAnim->target->set_fade(pAnim->fade_start + (int8_t)(fade_dis * p) );
		}


		if (anim_stop == true)
		{
			func_end_cb anim_end_cb = pAnim->anim_end_cb;
			gui_anim_delete(pAnim);
			if (anim_end_cb != GUI_ANIM_NULL_CB) anim_end_cb(pAnim->arg_ptr);
		}
		pAnim = pAnim->pNext;
	}
}