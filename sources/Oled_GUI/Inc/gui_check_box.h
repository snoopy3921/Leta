#ifndef __GUI_CHECK_BOX_H__
#define __GUI_CHECK_BOX_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"

enum CHKBOX_SIZE
{	
	CHKBOX_SIZE_1 	= 0,
	CHKBOX_SIZE_2	   , 
	CHKBOX_SIZE_3      ,
};

class gui_chkbox : public gui_obj
{
private:
    /* data */
public:
    gui_chkbox(/* args */){}
    ~gui_chkbox(){ destroy(); }

    void create(int16_t x, int16_t y, uint8_t size, gui_state state);
    void toggle();    
    void check()
    {
        this->add_state(GUI_STATE_CHECKED);
    }
    void uncheck()
    {
        this->clear_state(GUI_STATE_CHECKED);
    }
    bool is_checked()    
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
#endif /* __GUI_CHECK_BOX_H__ */