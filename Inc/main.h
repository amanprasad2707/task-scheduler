#ifndef MAIN_H_
#define MAIN_H_

#include "regs.h"

#define MAX_TASKS               5 // 4 user tasks and 1 idle task

#define DUMMY_XPSR              0x01000000U

#define INTERRUPT_DISABLE()    do{  __asm volatile("MOV R0, #0x1"); __asm volatile("MSR PRIMASK, R0"); } while (0)
#define INTERRUPT_ENABLE()    do{  __asm volatile("MOV R0, #0x0"); __asm volatile("MSR PRIMASK, R0"); } while (0)


/* some stack memory calculations */
#define SIZE_TASK_STACK         1024U
#define SIZE_SCHED_STACK        1024U

#define SRAM_START              0X20000000U
#define SRAM_SIZE               ((128) * (1024))
#define SRAM_END                ((SRAM_START) + (SRAM_SIZE))

#define T1_STACK_START          SRAM_END
#define T2_STACK_START          (SRAM_END - (1 * SIZE_TASK_STACK))
#define T3_STACK_START          (SRAM_END - (2 * SIZE_TASK_STACK))
#define T4_STACK_START          (SRAM_END - (3 * SIZE_TASK_STACK))
#define IDLE_STACK_START        (SRAM_END - (4 * SIZE_TASK_STACK))

#define SCHED_STACK_START       (SRAM_END - (5 * SIZE_TASK_STACK))



#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ


#define TASK_STATE_READY      0x00
#define TASK_STATE_BLOCKED      0xFF


#define EXC_RETURN_THREAD_PSP_NOFP        0xFFFFFFFD


#endif /* MAIN_H_ */
