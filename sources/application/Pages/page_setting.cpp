#include "gui.h"
#include "os_kernel.h"
#include "os_task.h"
#include "main.h"
#include "gui_port.h"


static uint8_t const ICON[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xf8, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x60, 0x60, 0x00, 0x8f, 
	0x8f, 0x00, 0x00, 0x00, 0x83, 0x83, 0x8c, 0x8c, 0x80, 0xe0, 0xe0, 0xff, 0xff, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x30, 0x30, 0x0c, 
	0x0c, 0x00, 0x01, 0x01, 0x80, 0x80, 0x00, 0x31, 0x31, 0x0c, 0x0c, 0x00, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x06, 0x06, 0x06, 
	0x06, 0x06, 0x31, 0x31, 0x00, 0xc6, 0xc6, 0x30, 0x30, 0x00, 0x06, 0x06, 0x01, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1f, 0x1f, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	/* (40 X 40 )*/
};


static gui_label lbl_setting[5];
static gui_label lbl_time[3];
static gui_label lbl_day[3];
static gui_group grp1;
static gui_anim anim1;
static gui_popup pu1;

static RTCDateTime time_set;

const char* item[] = 
{
	{"Screen time\0"},
	{"Brightness\0"},
	{"Invert\0"}, 
};

static void setup(void * p_arg)
{
	grp1.create(0, 0, 128, 64);
	for(uint8_t i = 0; i < 3; i++ )
	{
		lbl_setting[i].create(2, 2 + i * 20);
		lbl_setting[i].printf(item[i]);
		grp1.add_obj(&lbl_setting[i], GUI_FLAG_FOCUSABLE);
	}
	lbl_setting[3].create(2,62);
	grp1.add_obj(&lbl_setting[3], GUI_FLAG_FOCUSABLE);
	lbl_setting[3].printf("Set time:   :  :  \0");
	lbl_time[0].create(62,62);
	lbl_time[0].printf("%02d", watch_time_now.hour);
	lbl_time[1].create(80,62);
	lbl_time[1].printf("%02d", watch_time_now.minute);
	lbl_time[2].create(98,62);
	lbl_time[2].printf("%02d", watch_time_now.second);

	grp1.add_obj(&lbl_time[0], GUI_FLAG_DEFAULT);
	grp1.add_obj(&lbl_time[1], GUI_FLAG_DEFAULT);
	grp1.add_obj(&lbl_time[2], GUI_FLAG_DEFAULT);

	lbl_setting[4].create(2,82);
	grp1.add_obj(&lbl_setting[4], GUI_FLAG_FOCUSABLE);
	lbl_setting[4].printf("Set day:   /  /    \0");
	lbl_day[0].create(56,82);
	lbl_day[0].printf("%02d", watch_time_now.day);
	lbl_day[1].create(74,82);
	lbl_day[1].printf("%02d", watch_time_now.month);
	lbl_day[2].create(92,82);
	lbl_day[2].printf("%04d", watch_time_now.year);

	grp1.add_obj(&lbl_day[0], GUI_FLAG_DEFAULT);
	grp1.add_obj(&lbl_day[1], GUI_FLAG_DEFAULT);
	grp1.add_obj(&lbl_day[2], GUI_FLAG_DEFAULT);

	pu1.create(100, 50);
	anim1.create_fade(&grp1, 9, 0, 300);
	time_set = watch_time_now;
	os_task_delay(350);
}


static void loop(void * p_arg)
{
	os_task_delay(1);
	lbl_day[0].printf("%02d", time_set.day);
	lbl_day[1].printf("%02d", time_set.month);
	lbl_day[2].printf("%04d", time_set.year);
	lbl_time[0].printf("%02d", time_set.hour);
	lbl_time[1].printf("%02d", time_set.minute);
	lbl_time[2].printf("%02d", time_set.second);
}

static void delete_obj(void *p_arg)
{

	lbl_setting[0].destroy();
	lbl_setting[1].destroy();
	lbl_setting[2].destroy();
	lbl_setting[3].destroy();
	lbl_setting[4].destroy();

	lbl_day[0].destroy();
	lbl_day[1].destroy();
	lbl_day[2].destroy();
	lbl_time[0].destroy();
	lbl_time[1].destroy();
	lbl_time[2].destroy();
	pu1.destroy();
	grp1.destroy();
}


static void event_hdler(void * p_event)
{
	msg_t *p_msg = (msg_t *)p_event;
	switch (p_msg->sig)
	{
    case SIG_BTN_DR_PRESSED:
		/* code */
		os_timer_set_period(p_timer_countdown_screen_time, config.screen_time * 1000);
		os_timer_reset(p_timer_countdown_screen_time);
		if(time_set.year + time_set.month + time_set.day + time_set.hour + time_set.minute != \
			watch_time_now.year + watch_time_now.month + watch_time_now.day + watch_time_now.hour + watch_time_now.minute)
		{
			watch_time.setDateTime(time_set.year, time_set.month, time_set.day, time_set.hour, time_set.minute, time_set.second);
			watch_time_now = watch_time.getDateTime();
		}

		pu1.hide();
		anim1.create_move_offset(&grp1, 0, 64, 300);
		anim1.set_anim_end_cb(delete_obj, NULL);
		page_jump_to(PAGE_WATCH_ID);
		break;
    case SIG_BTN_UL_PRESSED:
		/* code */
		if(!pu1.is_popup_showed())
		{
			grp1.focus_obj(FOCUS_PREV);
			
		}
		else
		{
			switch (grp1.get_focus_index())
			{
			case 0: /* Screen time */
				config.screen_time += 1;
				config.screen_time = (config.screen_time > 5) ? config.screen_time : 5;
				pu1.get_label()->printf("Min: 5 s\r\nMax: 30 s\r\n\nCurrent: %d s", config.screen_time);
				break;
			case 1: /* Brightness */
				config.screen_brightness += 10;
				config.screen_brightness = (config.screen_brightness > 40) ? config.screen_brightness : 40;
				oled_set_brightness(config.screen_brightness);
				pu1.get_label()->printf("Min: 10\r\nMax: 255\r\n\nCurrent: %d", config.screen_brightness);
				break;
			case 2: /* Invert */
				config.screen_inverse = !config.screen_inverse;
				oled_invert(config.screen_inverse);
				pu1.get_label()->printf("Min: 0\r\nMax: 1\r\n\nCurrent: %d", config.screen_inverse);
				break;
			case 4: /* Hour */
				time_set.hour += 1;
				time_set.hour = (time_set.hour > 23) ? 0 : time_set.hour;
				pu1.get_label()->printf("Min: 0\r\nMax: 23\r\n\nCurrent: %d", time_set.hour);
				break;
			case 5: /* Minute */
				time_set.minute += 1;
				time_set.minute = (time_set.minute > 59) ? 0 : time_set.minute;
				pu1.get_label()->printf("Min: 0\r\nMax: 59\r\n\nCurrent: %d", time_set.minute);
				break;
			case 6: /* Second */
				time_set.second += 1;
				time_set.second = (time_set.second > 59) ? 0 : time_set.second;
				pu1.get_label()->printf("Min: 0\r\nMax: 59\r\n\nCurrent: %d", time_set.second);
				break;
			case 8: /* Day */
				time_set.day += 1;
				time_set.day = (time_set.day > day_of_month[time_set.month]) ? 1 : time_set.day;
				pu1.get_label()->printf("Min: 1\r\nMax: 31\r\n\nCurrent: %d", time_set.day);
				break;
			case 9: /* Month */
				time_set.month += 1;
				time_set.month = (time_set.month > 12 ) ? 1 : time_set.month;
				pu1.get_label()->printf("Min: 1\r\nMax: 12\r\n\nCurrent: %d", time_set.month);
				break;
			case 10: /* Year */
				time_set.year += 1;
				time_set.year = (time_set.year > 2100) ? 2021 : time_set.year;
				pu1.get_label()->printf("Min: 2021\r\nMax: 2100\r\n\nCurrent: %d", time_set.year);
				break;
			default:
				break;
			}
		}

		break;
    case SIG_BTN_DL_PRESSED:
		/* code */
		if(!pu1.is_popup_showed())
		{
			grp1.focus_obj(FOCUS_NEXT);
		}
		else
		{
			switch (grp1.get_focus_index())
			{
			case 0: /* Screen time */
				config.screen_time -= 1;
				config.screen_time = (config.screen_time < 5) ? 30 : config.screen_time;
				pu1.get_label()->printf("Min: 5 s\r\nMax: 30 s\r\n\nCurrent: %d s", config.screen_time);
				break;
			case 1: /* Brightness */
				config.screen_brightness -= 10;
				config.screen_brightness = (config.screen_brightness < 40) ? 255 : config.screen_brightness;
				oled_set_brightness(config.screen_brightness);
				pu1.get_label()->printf("Min: 10\r\nMax: 255\r\n\nCurrent: %d", config.screen_brightness);
				break;
			case 2: /* Invert */
				config.screen_inverse = !config.screen_inverse;
				oled_invert(config.screen_inverse);
				pu1.get_label()->printf("Min: 0\r\nMax: 1\r\n\nCurrent: %d", config.screen_inverse);
				break;
			case 4: /* Hour */
				time_set.hour -= 1;
				time_set.hour = (time_set.hour < 0) ? 23 : time_set.hour;
				pu1.get_label()->printf("Min: 0\r\nMax: 23\r\n\nCurrent: %d", time_set.hour);
				break;
			case 5: /* Minute */
				time_set.minute -= 1;
				time_set.minute = (time_set.minute < 0 ) ? 59 : time_set.minute;
				pu1.get_label()->printf("Min: 0\r\nMax: 59\r\n\nCurrent: %d", time_set.minute);
				break;
			case 6: /* Second */
				time_set.second -= 1;
				time_set.second = (time_set.second < 0) ? 59 : time_set.second;
				pu1.get_label()->printf("Min: 0\r\nMax: 59\r\n\nCurrent: %d", time_set.second);
				break;
			case 8: /* Day */
				time_set.day -= 1;
				time_set.day = (time_set.day < 1) ? day_of_month[time_set.month] : time_set.day;
				pu1.get_label()->printf("Min: 1\r\nMax: 31\r\n\nCurrent: %d", time_set.day);
				break;
			case 9: /* Month */
				time_set.month -= 1;
				time_set.month = (time_set.month < 1 ) ? 12 : time_set.month;
				pu1.get_label()->printf("Min: 1\r\nMax: 12\r\n\nCurrent: %d", time_set.month);
				break;
			case 10: /* Year */
				time_set.year -= 1;
				time_set.year = (time_set.year < 2021) ? 2100 : time_set.year;
				pu1.get_label()->printf("Min: 2021\r\nMax: 2100\r\n\nCurrent: %d", time_set.year);
				break;
			default:
				break;
			}
		}		

		break;
    case SIG_BTN_UR_PRESSED:
		/* code */

		switch (grp1.get_focus_index())
		{
		case 0: /* Screen time */
			pu1.toggle();
			pu1.get_label()->printf("Min: 5 s\r\nMax: 30 s\r\n\nCurrent: %d s", config.screen_time);
			break;
		case 1: /* Brightness */
			pu1.toggle();
			pu1.get_label()->printf("Min: 10\r\nMax: 255\r\n\nCurrent: %d", config.screen_brightness);
			break;
		case 2: /* Invert */
			pu1.toggle();
			pu1.get_label()->printf("Min: 0\r\nMax: 1\r\n\nCurrent: %d", config.screen_inverse);
			break;
		case 3: /* Set time */
			if(!lbl_time[0].check_flag(GUI_FLAG_FOCUSABLE))
			{
				lbl_time[0].add_flag(GUI_FLAG_FOCUSABLE);
				lbl_time[1].add_flag(GUI_FLAG_FOCUSABLE);
				lbl_time[2].add_flag(GUI_FLAG_FOCUSABLE);
				grp1.focus_obj(FOCUS_NEXT);
			}
			else
			{
				lbl_time[0].clear_flag(GUI_FLAG_FOCUSABLE);
				lbl_time[1].clear_flag(GUI_FLAG_FOCUSABLE);
				lbl_time[2].clear_flag(GUI_FLAG_FOCUSABLE);
			}
			break;
		case 7: /* Set day */
			if(!lbl_day[0].check_flag(GUI_FLAG_FOCUSABLE))
			{
				lbl_day[0].add_flag(GUI_FLAG_FOCUSABLE);
				lbl_day[1].add_flag(GUI_FLAG_FOCUSABLE);
				lbl_day[2].add_flag(GUI_FLAG_FOCUSABLE);
				grp1.focus_obj(FOCUS_NEXT);
			}
			else
			{
				lbl_day[0].clear_flag(GUI_FLAG_FOCUSABLE);
				lbl_day[1].clear_flag(GUI_FLAG_FOCUSABLE);
				lbl_day[2].clear_flag(GUI_FLAG_FOCUSABLE);
			}
			break;
		case 4: /* Hour */
			pu1.toggle();
			pu1.get_label()->printf("Min: 0\r\nMax: 23\r\n\nCurrent: %d", watch_time_now.hour);
			break;
		case 5: /* Minute */
			pu1.toggle();
			pu1.get_label()->printf("Min: 0\r\nMax: 23\r\n\nCurrent: %d", watch_time_now.minute);
			break;
		case 6: /* Second */
			pu1.toggle();
			pu1.get_label()->printf("Min: 0\r\nMax: 23\r\n\nCurrent: %d", watch_time_now.second);
			break;
		case 8: /* Day */
			pu1.toggle();
			pu1.get_label()->printf("Min: 1\r\nMax: 31\r\n\nCurrent: %d", watch_time_now.day);
			break;
		case 9: /* Month */
			pu1.toggle();
			pu1.get_label()->printf("Min: 1\r\nMax: 12\r\n\nCurrent: %d", watch_time_now.month);
			break;
		case 10: /* Year */
			pu1.toggle();
			pu1.get_label()->printf("Min: 2021\r\nMax: 2100\r\n\nCurrent: %d", watch_time_now.year);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
void page_setting_reg(void)
{
	page_create(PAGE_SETTING_ID, "Setting", ICON, setup, loop, event_hdler);
}
