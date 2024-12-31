/*
 * os_prio.h
 *
 *  Created on: Jun 26, 2024
 *      Author: giahu
 */

#ifndef OS_PRIO_H
#define OS_PRIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdio.h>
#include "os_cfg.h"

#define OS_PRIO_TBL_SIZE (((OS_CFG_PRIO_MAX - 1u) / (8u)) + 1u)

    void os_prio_init(void);
    void os_prio_insert(uint32_t prio);
    void os_prio_remove(uint32_t prio);
    uint32_t os_prio_get_highest(void);
    uint32_t os_prio_get_curr(void);

#ifdef __cplusplus
}
#endif
#endif /* OS_PRIO_H */
