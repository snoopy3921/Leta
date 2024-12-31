#ifndef __GUI_SLIDER_H__
#define __GUI_SLIDER_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"

typedef enum
{
    SLIDER_HOR = 0,
    SLIDER_VER,
}slider_type;


class gui_slider : public gui_obj
{
private:
    /* data */
	int16_t min;
	int16_t max;
	int16_t value;
    slider_type type;
public:
    gui_slider(/* args */){}
    ~gui_slider(){ destroy(); }

    void create(int16_t x, int16_t y, int16_t len, int16_t min, int16_t max, int16_t start_value, slider_type type);
    void set_value(int16_t val);
    int16_t get_value(void){ return this->value; }

    /* step > 0 to increase, and < 0 to decrease */
    void change_value_by_step(int16_t step){ this->set_value(value+=step); }
    void draw();
    void destroy()
    {
        gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this));
        this->deinit();
        min = max = value = 0;
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __GUI_SLIDER_H__ */
