#ifndef __MAIN_H__
#define __MAIN_H__


#include "os_kernel.h"
#include "os_mem.h"
#include "os_task.h"
#include "os_msg.h"
#include "os_timer.h"
#include "task_list.h"
#include "RTC.h"
#include "mpu_6050.h"
#include "button.h"
#include "io_cfg.h"

/*****************************************************************************/
/*  BUTTON task define
 */
/*****************************************************************************/

extern button_t btn_up_left;
extern button_t btn_up_right;
extern button_t btn_down_left;
extern button_t btn_down_right;

extern void btn_up_left_callback(void*  b);
extern void btn_up_right_callback(void*  b);
extern void btn_down_left_callback(void*  b);
extern void btn_down_right_callback(void*  b);

/* define signal */
enum {
	SIG_BTN_UL_PRESSED = 0,
	SIG_BTN_UL_LONG_PRESSED,
	SIG_BTN_UL_RELEASED,

	SIG_BTN_UR_PRESSED,
	SIG_BTN_UR_LONG_PRESSED,
	SIG_BTN_UR_RELEASED,

	SIG_BTN_DL_PRESSED,
	SIG_BTN_DL_LONG_PRESSED,
	SIG_BTN_DL_RELEASED,

	SIG_BTN_DR_PRESSED,
	SIG_BTN_DR_LONG_PRESSED,
	SIG_BTN_DR_RELEASED,
};

/*****************************************************************************/
/*  TIMER ID define
 */
/*****************************************************************************/
enum {
	TIMER_UPDATE_WATCH_TIME_ID = 0,
	TIMER_RELOAD_WATCH_DOG,
	TIMER_COUNT_DOWN_SCREEN_TIME,
};

extern os_timer_t *p_timer_countdown_screen_time;
extern void countdown_screen_time(void); 

extern RTCSTM32 watch_time;
extern RTCDateTime watch_time_now;
extern watch_config config;	
/**
 * 
 */


extern MPU6050_t mpu;
extern float bat_voltage;
extern float watch_temp;

extern int main();

#endif //__MAIN_H__
