#include "gui_cfg.h"


#if GUI_CFG_USE_PAGE_MANAGER == 1
#include "gui_page_manager.h"
#endif

#if GUI_CFG_USE_ANIM == 1
#include "gui_anim.h"
#endif

#if GUI_CFG_USE_SW == 1
#include "gui_sw.h"
#endif

#if GUI_CFG_USE_CHECK_BOX == 1
#include "gui_check_box.h"
#endif

#if GUI_CFG_USE_BITMAP == 1
#include "gui_bmp.h"
#endif

#if GUI_CFG_USE_FREE_STYLE == 1
#include "gui_free_style.h"
#endif

#if GUI_CFG_USE_LABEL == 1
#include "gui_label.h"
#endif

#if GUI_CFG_USE_SLIDER == 1
#include "gui_slider.h"
#endif

#if GUI_CFG_USE_IDCTOR == 1
#include "gui_idctor.h"
#endif

#if GUI_CFG_USE_POPUP == 1
#include "gui_popup.h"
#endif

#if GUI_CFG_USE_GROUP == 1
#include "gui_group.h"
#endif


extern void gui_initialize();

extern void gui_run();


