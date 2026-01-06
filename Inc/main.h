#ifndef MAIN_H_
#define MAIN_H_


#define MAX_TASKS               4

#define DUMMY_XPSR      0x01000000U


/* some stack memory calculations */
#define SIZE_TASK_STACK         1024U
#define SIZE_SCHED_STACK        1024U

#define SRAM_START              0X20000000U
#define SRAM_SIZE               ((128) * (1024))
#define SRAM_END                ((SRAM_START) + (SRAM_SIZE))

#define T1_STACK_START   SRAM_END
#define T2_STACK_START   (SRAM_END - (1 * SIZE_TASK_STACK))
#define T3_STACK_START   (SRAM_END - (2 * SIZE_TASK_STACK))
#define T4_STACK_START   (SRAM_END - (3 * SIZE_TASK_STACK))

#define SCHED_STACK_START (SRAM_END - (4 * SIZE_TASK_STACK))



#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ


#endif /* MAIN_H_ */
