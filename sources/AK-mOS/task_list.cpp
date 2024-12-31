#include "task_list.h"

const task_t app_task_table[] = {
    /*************************************************************************/
    /* TASK */
    /* TASK_ID          	task_func               arg     prio     msg_queue_size               stk_size */
    /*************************************************************************/
    {TASK_BTN_ID,   	    task_button,            NULL,   0,      0,                          100}, 
    {TASK_PAGE_MNG_ID,   	task_page_mng,          NULL,   1,      8,                          300},
    {TASK_GUI_ID,   	    task_gui,               NULL,   2,      8,                          100}, 


    

};
