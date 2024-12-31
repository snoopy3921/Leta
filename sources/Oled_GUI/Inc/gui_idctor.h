#ifndef __GUI_IDCTOR_H__
#define __GUI_IDCTOR_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"



class gui_idctor : public gui_obj
{
private:
    /* data */
	gui_obj* target;
    uint8_t gap;
public:
    gui_idctor(/* args */){}
    ~gui_idctor(){ destroy(); }

    void create(gui_obj* target, uint8_t gap);

    void set_gap(uint8_t gap){this->gap = gap;}
    uint8_t get_gap(void){return this->gap;}
    void set_target(gui_obj* target);
    gui_obj* get_target(){return this->target;}
    void draw();
    void destroy()
    {
        gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this));
        this->deinit();
        target = GUI_OBJ_NULL;
        gap = 0;
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __GUI_IDCTOR_H__ */
