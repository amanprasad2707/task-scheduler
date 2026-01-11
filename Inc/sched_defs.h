#ifndef SCHED_DEFS_H
#define SCHED_DEFS_H

#include <stdint.h>


#define MAX_TASKS               5 // 4 user tasks and 1 idle task

/* Task states */
#define TASK_STATE_READY    0x00
#define TASK_STATE_BLOCKED  0xFF


#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ



#endif
