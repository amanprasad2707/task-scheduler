#ifndef SCHED_DEFS_H
#define SCHED_DEFS_H

#include <stdint.h>


#define MAX_TASKS               5 // 4 user tasks and 1 idle task

#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ

/* ---------------- Task priorities ---------------- */
typedef enum{
    TASK_PRIORITY_HIGH = 0,
    TASK_PRIORITY_MEDIUM,
    TASK_PRIORITY_LOW,
    TASK_PRIORITY_IDLE
}task_priority_t;


/* ---------------- Task states ---------------- */
typedef enum {
    TASK_STATE_READY = 0,
    TASK_STATE_BLOCKED
} task_state_t;

/* ---------------- Task Control Block ---------------- */
typedef struct{
    uint32_t psp_value;
    uint32_t block_count;
    task_state_t current_state;
    task_priority_t priority;
    void (* task_handler)(void);    // function pointer to hold the task handler address
}TCB_t;


typedef enum{
    SCHED_RR,
    SCHED_PRIORITY,
}sched_algo_t;


#endif
