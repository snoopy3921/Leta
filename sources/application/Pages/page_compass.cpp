#include "gui.h"
#include "os_kernel.h"
#include "os_task.h"
#include "main.h"
#include "math.h"
#include "hmc5883l.h"
#include "mpu_6050.h"
static uint8_t const ICON[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x60, 
	0x60, 0x60, 0xe0, 0xe0, 0xe0, 0xe0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 
	0x0c, 0x0c, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x83, 0x83, 0x80, 0x00, 0x60, 
	0x60, 0xe0, 0xe0, 0x00, 0x03, 0x03, 0x0c, 0x0c, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x3c, 0x18, 0x18, 0x00, 0x00, 0x00, 0x80, 0x80, 0x3c, 
	0x3c, 0x00, 0x81, 0x81, 0x81, 0x81, 0x00, 0x3c, 0x3c, 0x01, 0x01, 0x00, 0x00, 0x00, 0x18, 0x18, 
	0x3c, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 
	0x30, 0x30, 0xc0, 0xc0, 0x00, 0x07, 0x07, 0x06, 0x06, 0x00, 0x01, 0xc1, 0xc1, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x30, 0x30, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 
	0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t const ICON_N[] = {
	0x00, 0x00, 0x00, 0xfe, 0xfe, 0xfe, 0xfc, 0xf8, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe,
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x01, 0x07, 0x0f, 0x3f, 0x7e,
	0xf8, 0xf0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00
};
static uint8_t const ICON_E[] = {
	0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x07, 0x07, 0x07, 0x07, 
	0x07, 0x07, 0x07, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00
};
static uint8_t const ICON_S[] = {
	0x00, 0x00, 0x00, 0xe0, 0xf8, 0xfc, 0xfc, 0x8e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x3c, 0x3c, 0x38, 
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc1, 0xc3, 0x83, 0x07, 0x07, 0x07, 0x07, 0x0e, 
	0x0e, 0xde, 0xfc, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00
};
static uint8_t const ICON_W[] = {
	0x0e, 0xfe, 0xfe, 0xf8, 0x80, 0x00, 0x00, 0xe0, 0xfc, 0xfe, 0xfe, 0xfc, 0xe0, 0x00, 0x00, 0x80, 
	0xf8, 0xfe, 0xfe, 0x0e, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0xfe, 0xff, 0x0f, 0x00, 0x00, 0x0f, 
	0xff, 0xfe, 0xf8, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00
};


static gui_label lbl_x;
static gui_free_style fs1;


static HMC5883L_t compass;
static float heading;

#define R_COMPASS 72

#define R_CP 64


static void free_style_draw(gui_obj *p_obj)
{
	gui_window_draw_line(&window_screen_size, 64, 48, 48, 128, DRAW_STATE_ON);
	gui_window_draw_line(&window_screen_size, 64, 48, 80, 128, DRAW_STATE_ON);

	float angle = heading;
	int16_t coor_y;
	int16_t coor_x;
	for(uint8_t i = 0; i < 16; i ++)
	{
		coor_y = 96 - R_CP * cos(angle * 0.017453292);
		coor_x = 64 - R_CP * sin(angle * 0.017453292);
		if(i % 4 != 0) gui_window_draw_char(&window_screen_size, coor_x, coor_y, '*', DRAW_STATE_ON);
		angle += 22.5;
		if(angle > 360) angle -= 360;
	}

	angle = heading;
		if(angle > 360) angle -= 360;
	gui_window_draw_bmp(&window_screen_size, 54 - R_CP * sin(angle * 0.017453292), 86 - R_CP * cos(angle * 0.017453292),  20, 20, ICON_N, DRAW_STATE_ON);
	
	angle = heading + 90;
		if(angle > 360) angle -= 360;
	gui_window_draw_bmp(&window_screen_size, 54 - R_CP * sin(angle * 0.017453292), 86 - R_CP * cos(angle * 0.017453292), 20, 20, ICON_W, DRAW_STATE_ON);
	
	angle = heading + 180;
		if(angle > 360) angle -= 360;
	gui_window_draw_bmp(&window_screen_size, 54 - R_CP * sin(angle * 0.017453292), 86 - R_CP * cos(angle * 0.017453292), 20, 20, ICON_S, DRAW_STATE_ON);
	
	angle = heading + 270;
		if(angle > 360) angle -= 360;
	gui_window_draw_bmp(&window_screen_size, 54 - R_CP * sin(angle * 0.017453292), 86 - R_CP * cos(angle * 0.017453292), 20, 20, ICON_E, DRAW_STATE_ON);
}
static void setup(void * p_arg)
{
	HMC5883LInit();
	lbl_x.create(0, 0);
	fs1.create(0, 0, 128, 64, free_style_draw);

	os_timer_set_period(p_timer_countdown_screen_time, 30000);
	os_timer_reset(p_timer_countdown_screen_time);

}

static void loop(void * p_arg)
{
    os_task_delay(1);


	HMC5883LRead(&compass);

	heading =  HMC5883LCalculateHeading(&compass);
	lbl_x.printf("Mag heading: %.2f", heading);

	
}
static void event_hdler(void * p_event)
{
	msg_t *p_msg = (msg_t *)p_event;
	switch (p_msg->sig)
	{
    case SIG_BTN_DR_PRESSED:
		/* code */	
		lbl_x.destroy();
		fs1.destroy();
		os_timer_set_period(p_timer_countdown_screen_time, config.screen_time * 1000);
		os_timer_reset(p_timer_countdown_screen_time);
		page_jump_to(PAGE_WATCH_ID);
		break;
	default:
		break;
	}
}
void page_compass_reg(void)
{
	page_create(PAGE_COMPASS_ID, "Compass", ICON, setup, loop, event_hdler);
}