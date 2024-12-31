#ifndef __GUI_PORT_H__
#define __GUI_PORT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdint.h"
#include "gui_cfg.h"

#if GUI_CFG_USE_LOG == 1
#define GUI_LOG(msg)      gui_log((uint8_t *)__FILE__, __LINE__, (uint8_t *)msg)
    extern void gui_log(uint8_t *file, uint32_t line, const uint8_t *msg);  
#else  
#define GUI_LOG(msg) (void)0
#endif

extern uint32_t get_tick(void);

extern void oled_init(void);

extern void oled_on(void);

extern void oled_off(void); 

extern void oled_invert(uint8_t i);

extern void oled_set_brightness(uint8_t value);

extern void oled_update(const uint8_t * buffer); 


#ifdef __cplusplus
}
#endif

#endif
