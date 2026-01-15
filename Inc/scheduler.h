#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

/* Scheduler core */
void init_task_stack(void);
void schedule(void);

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





#endif /* SCHEDULER_H */
