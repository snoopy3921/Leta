#ifndef __GUI_FREE_STYLE_H__
#define __GUI_FREE_STYLE_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"


typedef void (*custom_draw)(gui_obj *);

class gui_free_style : public gui_obj
{
private:
    /* data */
    custom_draw custom_draw_func;
public:
    gui_free_style(/* args */){}
    ~gui_free_style(){ destroy(); }

    void create(int16_t x, int16_t y, int16_t w, int16_t h, custom_draw custom_draw_func);
    void draw();
    void destroy()
    {
        gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this));
        this->deinit();
        custom_draw_func = nullptr;
    }
};




#ifdef __cplusplus
}
#endif
#endif /* __GUI_FREE_STYLE_H__ */