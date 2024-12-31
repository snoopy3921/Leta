
#ifndef __GUI_PAGE_MANAGER_H__
#define __GUI_PAGE_MANAGER_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

typedef void(*reg_func_ptr)(void);

/*****************************************************************************/
/*  USER DEFINE SECTION
/* 	
/*****************************************************************************/

enum PAGE_ID{
	PAGE_WATCH_ID = 0,
	PAGE_MENU_ID,
	PAGE_TIMER_ID,
	PAGE_SETTING_ID,
	PAGE_MPU_ID,
	PAGE_COMPASS_ID,
	PAGE_RUNNER_GAME_ID, 

    /* EOP page ID (Size of page table)*/
	PAGE_EOP_ID,
};

extern void page_watch_reg(void);
extern void page_menu_reg(void);

extern void page_timer_reg(void);
extern void page_setting_reg(void);
extern void page_mpu_reg(void);
extern void page_compass_reg(void);
extern void page_runner_game_reg(void);

/*****************************************************************************/
/*  USER DEFINE SECTION END
/* 	
/*****************************************************************************/




/*****************************************************************************/
/*  PAGE MANAGER:: Below section used to manage screens
/*
/*****************************************************************************/
extern reg_func_ptr reg_list[PAGE_EOP_ID];
extern void gui_page_init();
extern void gui_page_run(void *p_event);

typedef void(*function_ptr)(void * p_arg);

typedef struct {
    uint8_t id;                     /* Page ID */
	uint8_t status;					/* Page status */
	char    *title_ptr;             /* Pointer to page title */
	uint8_t *icon_ptr;              /* Pointer to page icon bitmap */
	function_ptr setup_function; /* Init page */
    function_ptr loop_funtion;  /* Loop page */
    function_ptr event_function;    /* Event handle */
}gui_page;

extern gui_page page_list[PAGE_EOP_ID];
extern void page_create(uint8_t page_id, const char* p_title, const uint8_t* p_icon, function_ptr setup_cb, function_ptr loop_cb, function_ptr event_cb);
extern void page_jump_to(uint8_t page_id);


/*****************************************************************************/
/*  DECLARE: List of register function and 
/*
/*****************************************************************************/
enum PAGE_STT{
	PAGE_READY = 0,
	PAGE_RUNNING,
};



#ifdef __cplusplus
}
#endif
#endif /* __GUI_PAGE_MANAGER_H__ */