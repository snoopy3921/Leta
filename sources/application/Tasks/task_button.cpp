#include "task_list.h"
#include "button.h"
#include "main.h"
#include "os_msg.h"
#include "os_task.h"

#include "io_cfg.h"

button_t btn_up_left;
button_t btn_up_right;
button_t btn_down_left;
button_t btn_down_right;

void btn_up_left_callback(void* b) {
	os_timer_reset(p_timer_countdown_screen_time);
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
        os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_UL_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_UL_LONG_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_UL_RELEASED);		
	}
		break;

	default:
		break;
	}
}

void btn_up_right_callback(void* b) {
	os_timer_reset(p_timer_countdown_screen_time);
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
        os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_UR_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_UR_LONG_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_UR_RELEASED);
		
	}
		break;

	default:
		break;
	}
}

void btn_down_left_callback(void* b) {
	os_timer_reset(p_timer_countdown_screen_time);
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_DL_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_DL_LONG_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_DL_RELEASED);
	}
		break;

	default:
		break;
	}
}
void btn_down_right_callback(void*  b)
{
	os_timer_reset(p_timer_countdown_screen_time);
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_DR_PRESSED);
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_DR_LONG_PRESSED);
		config_data_write(&config);
		power_pin_off();
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
		os_task_post_msg_pure(TASK_PAGE_MNG_ID, SIG_BTN_DR_RELEASED);
	}
		break;

	default:
		break;
	}	
}

void task_button(void *p_arg)
{
    while (1)
    {
        button_timer_polling(&btn_up_left);
        button_timer_polling(&btn_up_right);
        button_timer_polling(&btn_down_left);
		button_timer_polling(&btn_down_right);
        os_task_delay(10);
    }    
}