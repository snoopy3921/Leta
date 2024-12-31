#include "gui.h"
#include "os_kernel.h"
#include "os_task.h"
#include "main.h"


static void setup(void * p_arg)
{

}


static void loop(void * p_arg)
{
    os_task_delay(1);
	
}
static void event_hdler(void * p_event)
{
	msg_t *p_msg = (msg_t *)p_event;
	switch (p_msg->sig)
	{

	default:
		break;
	}
}
void page_template_reg(void)
{
	page_create(PAGE_ID, nullptr, nullptr, setup, loop, event_hdler);
}
