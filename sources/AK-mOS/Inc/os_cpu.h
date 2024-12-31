#ifndef OS_CPU_H
#define OS_CPU_H

#include "stm32f10x.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

#define DISABLE_INTERRUPTS          { __asm inline("CPSID   I \n"); }
#define ENABLE_INTERRUPTS           { __asm inline("CPSIE   I \n"); }


#define  os_cpu_SVCHandler          SVC_Handler
#define  os_cpu_PendSVHandler       PendSV_Handler
#define  os_cpu_SysTickHandler      SysTick_Handler


/* Make PendSV and SysTick the lowest priority interrupts. */
#define os_cpu_setup_PendSV()       (*(uint32_t volatile *)0xE000ED20 |= (0xFFU << 16))
#define os_cpu_trigger_PendSV()     (*(uint32_t volatile *)0xE000ED04 = (1U << 28))

extern void os_cpu_systick_init_freq(uint32_t cpu_freq);

#endif