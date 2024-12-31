#ifndef __GUI_POPUP_H__
#define __GUI_POPUP_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"
#include "gui_anim.h"
#include "gui_label.h"

class gui_popup : public gui_obj
{
private:
    /* data */
    int16_t x_popup;
    int16_t y_popup;
    int16_t w_popup;
    int16_t h_popup;
    bool is_showed;
    gui_anim anim;
    gui_obj obj_backgnd;
    gui_label label;
public:
    gui_popup(/* args */){}
    ~gui_popup(){ destroy(); }

    void create(int16_t w, int16_t h);

    gui_label* get_label(){ return &(this->label); }

    void show();
    void hide();
    void toggle();
    bool is_popup_showed(){ return this->is_showed; }

    void draw();
    void destroy()
    {

        gui_obj::gui_obj_remove(&obj_backgnd);
        obj_backgnd.deinit();

        label.destroy();

        x_popup = y_popup = w_popup = h_popup = 0;
        is_showed = false;
        anim.destroy();

        gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this));
        this->deinit();
    }
};




#ifdef __cplusplus
}
#endif
#endif /* __GUI_POPUP_H__ */
