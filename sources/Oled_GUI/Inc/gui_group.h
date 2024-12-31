#ifndef __GUI_GROUP_H__
#define __GUI_GROUP_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"
#include "gui_idctor.h"
#include "gui_anim.h"

#define MAX_OBJ_MEMBER 20

enum FOCUS_DIR{
	FOCUS_PREV,
	FOCUS_NEXT
};

class gui_group : public gui_obj
{
private:
    /* data */
    window group_win;
    gui_obj *member[MAX_OBJ_MEMBER];
    uint8_t mem_sum;
    int8_t  focus_index;
    gui_anim anim;
    gui_anim anim1;
    gui_idctor idctor;
public:
    gui_group(/* args */){}
    ~gui_group(){ destroy(); }

    void create(int16_t x, int16_t y, int16_t w, int16_t h);

    void set_x(int16_t x);
    void set_y(int16_t y);
    void set_fade(uint8_t fade);

    window get_window(){ return group_win; }

    gui_idctor* get_idctor(){ return &(this->idctor); }

    gui_anim* get_anim(){ return &(this->anim); }

    void add_obj(gui_obj* obj, gui_flag is_obj_focusable);
    void focus_obj(uint8_t direction);

    void set_pos_offset(int16_t delta_x, int16_t delta_y);

    int8_t get_focus_index();
    gui_obj* get_focus_obj();

    void destroy()
    {
        group_win = window_screen_size;
        mem_sum = 0;
        for(uint8_t i = 0; i < MAX_OBJ_MEMBER; i++)
        {
            member[i] = GUI_OBJ_NULL;
        }
        focus_index = -1;
        if(idctor.get_target() != GUI_OBJ_NULL) idctor.destroy();
        anim1.destroy();
        anim.destroy();
        this->deinit();
    }
};




#ifdef __cplusplus
}
#endif
#endif /* __GUI_GROUP_H__ */
