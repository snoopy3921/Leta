#include "gui.h"
#include "gui_core.h"
#include "gui_port.h"

void gui_initialize()
{
    oled_init();
    gui_obj::gui_init();
#if GUI_CFG_USE_ANIM == 1
    gui_anim::gui_anim_init();
#endif
}

void gui_run()
{
    gui_obj::gui_obj_render();
#if GUI_CFG_USE_ANIM == 1
    gui_anim::gui_anim_run();
#endif

}


