/*
 * os_cfg.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 */

#ifndef OS_CFG_H
#define OS_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define OS_CFG_HEAP_SIZE                  ((size_t)1024 * 3u)
#define OS_CFG_PRIO_MAX                   (10)
#define OS_CFG_DELAY_MAX                  ((uint32_t)0xffffffffUL)

#define OS_CFG_NUM_OF_TASKS_MAX           (10u)
#define OS_CFG_TASK_STK_SIZE_MIN          ((size_t)32u) // In stack, equal to x 4 bytes
#define OS_CFG_TASK_STACK_FILL_BYTE       (0x5Au)
#define OS_CFG_TASK_MSG_Q_SIZE_NORMAL     (8u)

#define OS_CFG_MSG_POOL_SIZE              (32u)

#define OS_CFG_TIMER_POOL_SIZE            (8u)  /* Max num of timer */
#define OS_CFG_TIMER_TASK_PRI             (0u)  /* Recommend as high as possible */


#ifdef __cplusplus
}
#endif
#endif /* OS_CFG_H */