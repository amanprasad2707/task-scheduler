
#ifndef MEM_LAYOUT_H
#define MEM_LAYOUT_H

#define SRAM_START          0x20000000U
#define SRAM_SIZE           (128U * 1024U)
#define SRAM_END            (SRAM_START + SRAM_SIZE)

/* Stack sizes */
#define SIZE_TASK_STACK     1024U
#define SIZE_SCHED_STACK   1024U

/* Task stack layout */
#define T1_STACK_START      (SRAM_END)
#define T2_STACK_START      (SRAM_END - (1 * SIZE_TASK_STACK))
#define T3_STACK_START      (SRAM_END - (2 * SIZE_TASK_STACK))
#define T4_STACK_START      (SRAM_END - (3 * SIZE_TASK_STACK))
#define IDLE_STACK_START    (SRAM_END - (4 * SIZE_TASK_STACK))
#define SCHED_STACK_START   (SRAM_END - (5 * SIZE_TASK_STACK))


#endif
