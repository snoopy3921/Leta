#include "gui_port.h"
#include "os_task.h"
#include "sh1106.h"

sh1106 my_display;

#if GUI_CFG_USE_LOG == 1
#include "system.h"
void gui_log(uint8_t *file, uint32_t line, const uint8_t *msg)
{
    SYS_PRINT("GUI log: %s\r\nAt file: %s:%d\r\n", msg, file, line);
}
#endif  

uint32_t get_tick(void)
{
    uint32_t tick = os_task_get_tick();
    return tick;
}

void oled_init(void)
{
    my_display.init();
}

void oled_on(void)
{
    my_display.display_on();
}

void oled_off(void)
{
    my_display.display_off();
}

void oled_update(const uint8_t * buffer)
{
    my_display.display_buffer(buffer);
}

void oled_invert(uint8_t i)
{
    my_display.invertDisplay(i);
}

void oled_set_brightness(uint8_t value)
{
    my_display.setBrightness(value);
}