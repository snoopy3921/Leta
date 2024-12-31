#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "os_task.h"

extern const task_t app_task_table[];


enum
{
	/* SYSTEM TASKS */
	TASK_BTN_ID,
	TASK_PAGE_MNG_ID,
	TASK_GUI_ID,

	/* EOT task ID (Size of task table)*/
	TASK_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Task function
 */
/*****************************************************************************/
/* APP TASKS */
extern void task_page_mng(void *p_arg);
extern void task_button(void *p_arg);
extern void task_gui(void *p_arg);

#endif //__TASK_LIST_H__
