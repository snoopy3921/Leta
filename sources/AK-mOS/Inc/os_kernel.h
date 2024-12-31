/*
 * os_kernel.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 *
 *      THE KERNEL IS A PRE-DEFINE KERNEL THAT ALL THE TASKS HAVE TO CREATE
 * BEFORE KERNEL STARTS
 */

#ifndef OS_KERNEL_H
#define OS_KERNEL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "os_cfg.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#if OS_CFG_PRIO_MAX > 255u || OS_CFG_PRIO_MAX < 0u
#error OS_CFG_PRIO_MAX have to be between 0-255
#endif

#define OS_TRUE             ((uint8_t)1)
#define OS_FALSE            ((uint8_t)0)

#define os_assert(exp)      ((exp) ? (void)0 : assert_log((uint8_t *)__FILE__, __LINE__))

    extern void assert_log(uint8_t *file, uint32_t line);

    extern void os_critical_enter(void);
    extern void os_critical_exit(void);

    extern void os_init(void);
    extern void os_run(void);

#define ENTER_CRITICAL()    os_critical_enter()
#define EXIT_CRITICAL()     os_critical_exit()

#ifdef __cplusplus
}
#endif
#endif /* OS_KERNEL_H */
