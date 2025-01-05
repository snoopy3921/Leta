
#include "gui_page_manager.h"
#include "gui_port.h"
#include <stdint.h>


/*****************************************************************************/
/*  USER DEFINE SECTION
/* 	
/*****************************************************************************/

reg_func_ptr reg_list[PAGE_EOP_ID] = 
{
    page_watch_reg,
    page_menu_reg,
    page_timer_reg,
    page_setting_reg,
    page_flashing_reg, 
    page_mpu_reg,
    page_compass_reg,
    page_runner_game_reg,
};

/*****************************************************************************/
/*  USER DEFINE SECTION END
/* 	
/*****************************************************************************/





/*****************************************************************************/
/*  PAGE MANAGER: Below section used to manage screens
/*
/*****************************************************************************/
static uint8_t curr_page; 

gui_page page_list[PAGE_EOP_ID];

void page_create(uint8_t page_id, const char* p_title, const uint8_t* p_icon, function_ptr setup_cb, function_ptr loop_cb, function_ptr event_cb)
{
    page_list[page_id].title_ptr = (char *)p_title;
    page_list[page_id].icon_ptr = (uint8_t*)p_icon;
    //page_list[page_id].status = PAGE_READY;
    if(setup_cb == NULL || loop_cb == NULL || event_cb == NULL)
    {
        GUI_LOG("Functions call back could not be null");
    }
    else
    {
        page_list[page_id].setup_function = setup_cb;
        page_list[page_id].loop_funtion = loop_cb;
        page_list[page_id].event_function = event_cb;
    }
}

void page_jump_to(uint8_t page_id)
{
    if(curr_page == page_id) return;
    else
    {
        page_list[curr_page].status = PAGE_READY;
        curr_page = page_id;
        page_list[curr_page].status = PAGE_RUNNING;
        page_list[curr_page].setup_function(NULL);
    }
}

void gui_page_init()
{
    uint8_t idx = 0;
    reg_func_ptr func_temp;
    while (idx < PAGE_EOP_ID)
    {
        func_temp = reg_list[idx];
        func_temp();
        idx++;
    }  
    curr_page = 0;
    page_list[curr_page].status = PAGE_RUNNING;
    page_list[0].setup_function(NULL);
}

void gui_page_run(void *p_event)
{
    if(p_event != NULL) page_list[curr_page].event_function(p_event);
                
    if(page_list[curr_page].status == PAGE_RUNNING)
    {                  
        page_list[curr_page].loop_funtion(NULL);
    }

}
