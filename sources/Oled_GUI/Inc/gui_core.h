
#ifndef __GUI_CORE_H__
#define __GUI_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_cfg.h"
#include "gui_render.h"

#define GUI_TRUE             ((uint8_t)1)
#define GUI_FALSE            ((uint8_t)0)

#define gui_assert(exp)      ((exp) ? (void)0 : user_assert((uint8_t *)__FILE__, __LINE__))

extern void user_assert(uint8_t *file, uint32_t line);

#define GUI_OBJ_NULL ((gui_obj*)0)  

typedef enum
{
    GUI_FLAG_DEFAULT      = (1 << 0),
    GUI_FLAG_CLICKABLE    = (1 << 1),  /* Object is clickable (e.g., button) */
    GUI_FLAG_SCROLLABLE   = (1 << 2),  /* Object can be scrolled */
    GUI_FLAG_FOCUSABLE    = (1 << 3),  /* Object can be focus */
    //,..
} gui_flag;

typedef enum
{
    GUI_STATE_DEFAULT   = (1 << 0),
    GUI_STATE_CHECKED   = (1 << 1),     /*For checkbox, for switch*/
    GUI_STATE_DISABLE   = (1 << 2),     /*Object has disabled, will not be drawn*/
    GUI_STATE_FOCUSED   = (1 << 3),     /* Object is clicked (e.g., button) */
    //,..
} gui_state;

typedef enum 
{
    GUI_ALIGN_OFF   = 0,
    GUI_ALIGN_TOP  ,
    GUI_ALIGN_BOTTOM, 
    GUI_ALIGN_LEFT , 
    GUI_ALIGN_RIGHT , 
    GUI_ALIGN_HOR_MID , 
    GUI_ALIGN_VER_MID , 
    GUI_ALIGN_TOP_LEFT ,
    GUI_ALIGN_TOP_MID,
    GUI_ALIGN_TOP_RIGHT,
    GUI_ALIGN_BOTTOP_LEFT,
    GUI_ALIGN_BOTTOP_MID,
    GUI_ALIGN_BOTTOP_RIGHT,
} gui_align;

extern window window_screen_size;
extern uint32_t gui_get_tick_now(void);
extern uint32_t gui_get_tick_elap(uint32_t prev_tick);


class gui_obj
{
private:
    /* data */
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    window * window_ptr;

    gui_flag flag;
    
    gui_state state;

    uint8_t     fade;

    gui_align align;

    gui_obj   * next_obj_ptr;
    gui_obj   * prev_obj_ptr;

    void * feature_ptr;

protected:
    /*Gui_obj methods*/
    static gui_obj * obj_head_ptr;
    static gui_obj * obj_tail_ptr;
    static void gui_obj_init();
    static void gui_obj_add_head(gui_obj * obj);
    static void gui_obj_add_tail(gui_obj * obj);
    static void gui_obj_remove(gui_obj * obj);

    

public:
    gui_obj(/* args */){}
    ~gui_obj(){ destroy(); }
    void deinit()
    {
        x = y = w = h = fade = 0;
        flag = GUI_FLAG_DEFAULT;
        state = GUI_STATE_DEFAULT;
        next_obj_ptr = prev_obj_ptr = GUI_OBJ_NULL;
        feature_ptr = nullptr;
    }
    virtual void destroy(){ gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this)); }
    virtual void set_x(int16_t x) { this->x = x; }
	virtual void set_y(int16_t y) { this->y = y; }
	void set_w(int16_t w) { this->w = w; }
	void set_h(int16_t h) { this->h = h; }
    int16_t get_x(void) { return x; }
	int16_t get_y(void) { return y; }
	int16_t get_w(void) { return w; }
	int16_t get_h(void) { return h; }

    void set_feature(void *p_feature) { this->feature_ptr = p_feature; }
	void *get_feature(void) { return this->feature_ptr; }

    void set_window(window *p_win){ this->window_ptr = p_win; }
    window *get_window(void) { return this->window_ptr; }

    void set_allign(gui_align allign);

    bool check_state(gui_state state) { return (this->state & state) > 0; }
    bool check_flag(gui_flag flag) { return (this->flag & flag) > 0; }
	void add_state(gui_state state)
	{
		gui_state new_state = (gui_state)(this->state | state);
		if (new_state != this->state)
			this->state = new_state;
		if (new_state & GUI_STATE_DISABLE)
			this->state = GUI_STATE_DISABLE;
	}
	void clear_state(gui_state state)
	{
		gui_state new_state = (gui_state)(this->state & (~state));
		if (new_state != this->state)
			this->state = new_state;
	}
    void add_flag(gui_flag flag)
	{
		gui_flag new_flag = (gui_flag)(this->flag | flag);
		if (new_flag != this->flag)
			this->flag = new_flag;
	}
	void clear_flag(gui_flag flag)
	{
		gui_flag new_flag = (gui_flag)(this->flag & (~flag));
		if (new_flag != this->flag)
			this->flag = new_flag;
	}
    virtual void set_fade(uint8_t fade)
    {
        if(fade < 0) this->fade = 0;
        else if(fade > 9) this->fade = 9;
        else this->fade = fade;
    }
    uint8_t get_fade(void) { return this->fade; }
    void draw_fade(void);
    virtual void draw();


    /**
     * Static fields
     */
    static void gui_init();
    static void gui_obj_render();

};


#ifdef __cplusplus
}
#endif
#endif /* __GUI_CORE_H__ */
