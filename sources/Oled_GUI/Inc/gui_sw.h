#ifndef __GUI_SW_H__
#define __GUI_SW_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"

enum SW_SIZE
{	
	SW_SIZE_1 	= 0,
	SW_SIZE_2	   , 
	SW_SIZE_3      ,
};

class gui_sw : public gui_obj
{
private:
    /* data */
public:
    gui_sw(/* args */){}
    ~gui_sw(){ destroy(); }

    void create(int16_t x, int16_t y, uint8_t size, gui_state state);
    void toggle();
    void turn_on()
    {
        this->add_state(GUI_STATE_CHECKED);
    }
    void turn_off()
    {
        this->clear_state(GUI_STATE_CHECKED);
    }
    bool is_sw_on()
    {
        return this->check_state(GUI_STATE_CHECKED);
    }
    void draw();
    void destroy()
    {
        gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this));
        this->deinit();
    }
};




#ifdef __cplusplus
}
#endif
#endif /* __GUI_SW_H__ */
