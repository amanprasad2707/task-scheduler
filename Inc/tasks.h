#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#define RTOS_HEAP_SIZE  (8 * 1024)   // 8 KB total heap


/* Task function type */
typedef void (*task_func_t)(void *);


/* Task states */
typedef enum {
    TASK_STATE_UNUSED = 0,
    TASK_STATE_READY,
    TASK_STATE_BLOCKED,
    TASK_STATE_RUNNING
} task_state_t;


/* Task priorities */
typedef enum{
    TASK_PRIORITY_HIGH = 0,
    TASK_PRIORITY_MEDIUM,
    TASK_PRIORITY_LOW,
    TASK_PRIORITY_IDLE
}task_priority_t;


/* Task Control Block  */
typedef struct {
    uint32_t *psp;              // Saved PSP
    uint32_t block_count;       // Tick to unblock (for delay)

    uint8_t  *stack_base;       // Stack memory start
    uint32_t  stack_size;       // Stack size in bytes

    task_state_t state;
    task_priority_t priority;

    task_func_t entry;          // Task entry function
    void *arg;                  // Argument to task
} TCB_t;





int task_create(
    void (*task_fn)(void *),   // WHAT runs
    void *arg,                 // WITH what data
    uint32_t stack_size_bytes, // HOW MUCH stack
    task_priority_t priority   // HOW important
);


void idle_task(void);
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

#endif
