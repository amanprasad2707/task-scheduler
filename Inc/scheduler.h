#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define MAX_TASKS               10

#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ


typedef enum{
    SCHED_RR,
    SCHED_PRIORITY,
}sched_algo_t;

/* Scheduler core */
void schedule(void);
void scheduler_start(void);


/* Tick handling */
void update_global_tick_count(void);
void unblock_tasks(void);
void init_systick_timer(uint32_t tick_hz);

/* Task services */
void task_delay(uint32_t tick_count);

/* PendSV support */
void save_psp_value(uint32_t current_psp_val);
void update_next_task(void);
uint32_t get_psp_value(void);

__attribute__((naked)) void switch_sp_to_psp(void);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);

void task_set_priority(uint8_t task, task_priority_t task_priority);



#endif /* SCHEDULER_H */
