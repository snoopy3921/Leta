#ifndef __GUI_ANIM_H__
#define __GUI_ANIM_H__

#include "gui_core.h"   

typedef void(*func_end_cb)(void *p_arg);

#define GUI_ANIM_NULL_CB ((func_end_cb)0)
#define GUI_ANIM_NULL ((gui_anim*)0)

enum
{
    ANIM_ONE_SHOT = false,
    ANIM_INFINITY = true,
};
enum
{
    ANIM_READY = 0,
    ANIM_RUNNING = 1,
};

class gui_anim 
{
private:
    /* data */
    int16_t x_start;
    int16_t y_start;
    int16_t w_start;
    int16_t h_start;
    int16_t w_stop;         //Difference
    int16_t h_stop;
    int16_t x_stop;
    int16_t y_stop;
	int8_t fade_start;
	int8_t fade_stop;
    
    uint16_t duration;
    uint16_t act_time;      /**< Current time in animation. Set to negative to make delay.*/
    uint16_t playback_duration; 
    bool is_infinity;
    func_end_cb anim_end_cb;
    void *arg_ptr;
    uint8_t swap_cnt; /* For internal use */
    uint8_t state; /* For internal use */
    gui_anim *pPrev;
    gui_anim *pNext;
    gui_obj *target;

protected:
    static gui_anim* pFirst;
    static gui_anim* pLast;

public:
    gui_anim();

    ~gui_anim();

    void init();

    void create_move(gui_obj* target, int16_t x_stop, int16_t y_stop, uint16_t duration);
    void create_move_offset(gui_obj* target, int16_t delta_x, int16_t delta_y, uint16_t duration);
    void create_resize(gui_obj* target, int16_t x_stop, int16_t y_stop, int16_t w_stop, int16_t h_stop, uint16_t duration);
    void create_resize_offset(gui_obj* target, int16_t delta_x, int16_t delta_y, int16_t delta_w, int16_t delta_h, uint16_t duration);
    void create_fade(gui_obj* target, uint8_t fade_start, uint8_t fade_stop, uint16_t duration);
    void create_fade_offset(gui_obj* target, uint8_t delta_fade, uint16_t duration);
    void set_behavior(uint16_t playback_duration, bool is_infinity);
    void stop();

    void set_anim_end_cb(func_end_cb anim_end_cb, void *p_arg)
    { 
        this->anim_end_cb = anim_end_cb; 
        this->arg_ptr = p_arg;
    }

    void destroy()
    {
        stop();
        this->target = GUI_OBJ_NULL;
        this->x_start = -1;
        this->y_start = -1;
        this->x_stop = -1;
        this->y_stop = -1;
        this->duration = 0;
        this->act_time = 0;
        this->w_start = -1;
        this->fade_start = -1;
        this->anim_end_cb = GUI_ANIM_NULL_CB;
        this->arg_ptr = nullptr;
        this->swap_cnt = 0;
        this->state = ANIM_READY;
    }

    static void gui_anim_init(void);
    static void gui_anim_inverse_attribute(gui_anim *pAnim);
    static void gui_anim_add(gui_anim* pAnim);
    static void gui_anim_delete(gui_anim* pAnim);
    static void gui_anim_run(void);
    static float get_fps();
    
};

#define SIZE_OF_ANIM sizeof(oled_anim)

#endif