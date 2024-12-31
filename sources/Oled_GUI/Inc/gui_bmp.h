#ifndef __GUI_BMP_H__
#define __GUI_BMP_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"

#define MAX_TEXT_LEN 18

class gui_bmp : public gui_obj
{
private:
    /* data */	
public:
    gui_bmp(/* args */){}
    ~gui_bmp(){ destroy(); }

    void create(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *p_bmp);
    void set_bmp(const uint8_t* p_bmp){ this->set_feature((void *) p_bmp);}
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
#endif /* __GUI_BMP_H__ */
