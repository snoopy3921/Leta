/*
 * os_timer.h
 *
 *  Created on: Dec 6, 2024
 *      Author: giahu
 */

#ifndef OS_TIMER_H
#define OS_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "os_list.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

    typedef struct timer os_timer_t;
    typedef struct timer_pool timer_pool_t;
    typedef uint8_t timer_id_t;
    typedef void (*timer_cb)();

    typedef enum
    {
        TIMER_ONE_SHOT,
        TIMER_PERIODIC
    } timer_type_t;

    struct timer
    {
        os_timer_t *next;
        timer_id_t id;               /* Maybe for debugging */
        list_item_t timer_list_item; /* Value of this item plays as time stamp to trigger timer*/

        int32_t sig; /* Timer signal*/
        uint8_t des_task_id;
        timer_cb func_cb; /* Callback funtion runs on timer task, !!! keep it short and simple as in interrupt*/

        uint32_t period; /* In case one-shot timer, this field equals 0 */
        timer_type_t type;
    };

    void os_timer_init(void); /* Runs on kernel init */

    void os_timer_processing(); /* Runs on timer task */

    /* These APIs run on other tasks, where they are calling*/
    os_timer_t *os_timer_create(timer_id_t id, int32_t sig, timer_cb func_cb, uint8_t des_task_id, uint32_t period, timer_type_t type);

    void os_timer_set_period(os_timer_t *p_timer, uint32_t period);

    void os_timer_start(os_timer_t *p_timer, uint32_t tick_to_wait);
    void os_timer_reset(os_timer_t *p_timer);
    void os_timer_remove(os_timer_t *p_timer);

#ifdef __cplusplus
}
#endif
#endif /* OS_TIMER_H */
