#ifndef __GUI_LABEL_H__
#define __GUI_LABEL_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "gui_core.h"

#define MAX_TEXT_LEN 50

class gui_label : public gui_obj
{
private:
    /* data */
    char text[MAX_TEXT_LEN];		
public:
    gui_label(/* args */){}
    ~gui_label(){ destroy(); }

    void create(int16_t x, int16_t y);
    void printf(const char *str, ...);
    void clear();
    void draw();
    void destroy()
    {
        gui_obj::gui_obj_remove(dynamic_cast<gui_obj*>(this));
        this->deinit();
        text[0] = '\0';
    }
};

#ifdef __cplusplus
}
#endif
#endif /* __GUI_LABEL_H__ */
