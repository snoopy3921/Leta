#include "gui.h"
#include "os_kernel.h"
#include "os_task.h"
#include "main.h"



#define ICON_SUM (PAGE_EOP_ID - PAGE_MENU_ID - 1)
#define	ICON_GAP    72
static uint8_t page_now;

static gui_group grp1;
static gui_anim anim1, anim2, anim3;
static gui_label lbl_title, lbl_time;
static gui_bmp bmp[ICON_SUM];

static void setup(void * p_arg)
{
	uint8_t i;
    page_now  = PAGE_MENU_ID + 1;
	grp1.create(0, 0, 128, 64);
	for(i = 0; i < ICON_SUM; i++)
	{
		bmp[i].create(i * ICON_GAP + 42, 12, 40, 40, page_list[i + PAGE_MENU_ID + 1].icon_ptr);
        grp1.add_obj(&bmp[i], GUI_FLAG_DEFAULT);
	}
    lbl_title.create(48, 56 + 64);
    lbl_title.printf("%s", page_list[page_now].title_ptr);

	lbl_time.create(0, 64);
	lbl_time.printf("%02d:%02d", watch_time_now.hour, watch_time_now.minute);


    grp1.set_pos_offset(0, 64);
    anim2.create_move_offset(&lbl_title, 0, -64, 300);
	anim3.create_move_offset(&lbl_time, 0, -64, 300);
    anim1.create_move_offset(&grp1, 0, -64, 300);
    os_task_delay(350);
}


static void loop(void * p_arg)
{
    os_task_delay(1);
	lbl_time.printf("%02d:%02d", watch_time_now.hour, watch_time_now.minute);
}

static void delete_obj(void *p_arg)
{
	for(uint8_t i = 0; i < ICON_SUM; i++)
	{
		bmp[i].destroy();
	}
    lbl_title.destroy();
	lbl_time.destroy();
    grp1.destroy();
    
}
static void event_hdler(void * p_event)
{
    msg_t *p_msg = (msg_t *)p_event;
	switch (p_msg->sig)
	{
	case SIG_BTN_UR_PRESSED:
        page_now++;
        if(page_now == PAGE_EOP_ID)
		{
			page_now = PAGE_MENU_ID + 1;
            anim1.create_move_offset(&grp1, ICON_GAP * (ICON_SUM - 1), 0, 150);
            os_task_delay(150);
		}
		else
        {
			anim1.create_move_offset(&grp1, -1 * ICON_GAP, 0, 150);
            os_task_delay(150);
        }
        lbl_title.printf("%s", page_list[page_now].title_ptr);
        lbl_title.set_allign(GUI_ALIGN_BOTTOP_MID);
        anim2.create_fade(&lbl_title, 9, 0, 150);
		break;
    case SIG_BTN_UL_PRESSED:
        page_now--;
    	if(page_now == PAGE_MENU_ID)
		{
			page_now = PAGE_EOP_ID - 1;
			anim1.create_move_offset(&grp1, -ICON_GAP * (ICON_SUM - 1), 0, 150);		
            os_task_delay(150);
		}
		else
        {
            anim1.create_move_offset(&grp1, ICON_GAP, 0, 150);
            os_task_delay(150);
        }
        lbl_title.printf("%s", page_list[page_now].title_ptr);
        lbl_title.set_allign(GUI_ALIGN_BOTTOP_MID);
        anim2.create_fade(&lbl_title, 9, 0, 150);
        break;
    case SIG_BTN_DR_PRESSED:
		/* code */
        grp1.add_obj(&lbl_title, GUI_FLAG_DEFAULT);
		anim1.create_move_offset(&grp1, 0, 64, 300);
		anim1.set_anim_end_cb(delete_obj, NULL);
		page_jump_to(PAGE_WATCH_ID);
		break;
    case SIG_BTN_DL_PRESSED:
		/* code */
		anim3.create_move_offset(&lbl_time, 0, 64, 200);
		anim2.create_move_offset(&lbl_title, 0, 64, 200);
		anim1.create_move_offset(&grp1, 0, 64, 300);        
		anim1.set_anim_end_cb(delete_obj, NULL);
		page_jump_to(page_now);
		break;
	default:

		break;
	}
}
void page_menu_reg(void)
{
	page_create(PAGE_MENU_ID, nullptr, nullptr, setup, loop, event_hdler);
}
