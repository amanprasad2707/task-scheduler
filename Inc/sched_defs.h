#ifndef SCHED_DEFS_H
#define SCHED_DEFS_H

#include <stdint.h>


#define MAX_TASKS               10

#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ









typedef enum{
    SCHED_RR,
    SCHED_PRIORITY,
}sched_algo_t;


#endif
