#include "os_cpu.h"
#include "os_task.h"
#include "os_kernel.h"


#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

/*Passing SystemCoreClock to init tick at every 1ms*/
void os_cpu_systick_init_freq(uint32_t cpu_freq)
{
	volatile uint32_t ticks = cpu_freq / 1000;

	SysTick->LOAD = ticks - 1; /* set reload register */

	NVIC_SetPriority(SysTick_IRQn, 0);

	SysTick->VAL = 0; /* Load the SysTick Counter Value */

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
}

#ifdef __cplusplus
extern "C"
{
#endif
	void os_cpu_SVCHandler(void)
	{
		__asm volatile(
		    "CPSID   I				\n" // Prevent interruption during context switch
		    "LDR     R1, =tcb_curr_ptr 	\n" // get pointer to TCB current
		    "LDR     R1, [R1]			\n" // get TCB current	= pointer to StkPtr
		    "LDR     R0, [R1]             	\n" // get StkPtr
		    "LDMIA R0!, {R4-R11}        	\n" //
		    "MSR     PSP, R0              	\n" //
		    "ORR LR, #0xD                	\n" // LR = 0xFFFFFFFD return to threadmode
		    "CPSIE   I				\n" //
		    "BX      LR				\n" //
		);
	}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if 1
	void os_cpu_PendSVHandler(void)
	{
		__asm volatile(
		    //"CPSID   I                       		\n"	//Prevent interruption during context switch
		    "MRS     R0, PSP                     		\n" // PSP is process stack pointer
		    "CBZ     R0, OS_CPU_PendSVHandler_nosave   	\n" // Skip register save the first time

		    "SUBS    R0, R0, #0x20               		\n" // Save remaining regs r4-11 on process stack
		    "STM     R0, {R4-R11}				\n" //

		    "LDR     R1, =tcb_curr_ptr               	\n" // OSTCBCur->OSTCBStkPtr = SP;
		    "LDR     R1, [R1]\n"				    //
		    "STR     R0, [R1]                    		\n" // R0 is SP of process being switched out

		    /* At this point, entire context of process has been saved	*/

		    "OS_CPU_PendSVHandler_nosave:			\n" //
		    "LDR     R0, =tcb_curr_ptr               	\n" // OSTCBCur  = OSTCBHighRdy;
		    "LDR     R1, =tcb_high_rdy_ptr			\n" //
		    "LDR     R2, [R1]					\n" //
		    "STR     R2, [R0]					\n" //

		    "LDR     R0, [R2]                    		\n" // R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
		    "LDM     R0, {R4-R11}                		\n" // Restore r4-11 from new process stack
		    "ADDS    R0, R0, #0x20				\n" //
		    "MSR     PSP, R0                     		\n" // Load PSP with new process SP
		    "ORR     LR, LR, #0x04               		\n" // Ensure exception return uses process stack
		    //"CPSIE   I						\n"  	//
		    "BX      LR                          		\n" // Exception return will restore remaining context
		);
	}
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	void os_cpu_SysTickHandler()
	{
		DISABLE_INTERRUPTS
		/* Increment the RTOS tick. */
		if (os_task_increment_tick() == OS_TRUE)
		{
			/* A context switch is required.  Context switching is performed in
			 * the PendSV interrupt.  Pend the PendSV interrupt. */
			os_cpu_trigger_PendSV();
		}
		ENABLE_INTERRUPTS
	}

#ifdef __cplusplus
}
#endif
