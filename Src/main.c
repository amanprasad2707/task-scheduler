#include <stdint.h>
#include <stdio.h>
#include "main.h"


/* This is a global array that holds the PSP of different tasks */
uint32_t psp_of_tasks[MAX_TASKS] = {T1_STACK_START, T2_STACK_START, T3_STACK_START, T4_STACK_START};
uint32_t task_handlers[MAX_TASKS];


void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_task_stack(void);


int main(void){
    init_scheduler_stack(SCHED_STACK_START);

    task_handlers[0] = (uint32_t) task1_handler;
    task_handlers[1] = (uint32_t) task2_handler;
    task_handlers[2] = (uint32_t) task3_handler;
    task_handlers[3] = (uint32_t) task4_handler;

    init_task_stack();

    init_systick_timer(TICK_HZ);
    /* Loop forever */
	for(;;);
}


void task1_handler(void){
    while(1){
        printf("this is task 1\n");
        
    }
}

void task2_handler(void){
    while(1){
        printf("this is task 2\n");
}

}

void task3_handler(void){
    while(1){
        printf("this is task 3\n");
        
    }
    
}

void task4_handler(void){
    while(1){
        printf("this is task 4\n");
        
    }
    
}


void init_systick_timer(uint32_t tick_hz){
    uint32_t *pSRVR = (uint32_t *) 0xE000E014; // systick relaod value register address
    uint32_t *pSCSR = (uint32_t *) 0xE000E010; // systick control and status register address
    uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1;

    // clear the value of SRVR
    *pSRVR &= ~(0x00FFFFFF);

    // load the value in the SRVR
    *pSRVR |= count_value;

    // do some systick config
    *pSCSR |= (1 << 1); // enables systick exception request
    *pSCSR |= (1 << 2); // sets the processors clock source (not external)
    *pSCSR |= (1 << 0); // enables the counter
}

void SysTick_Handler(void){
    
}


// This is a naked function so no prologue and epilogue. That's why we have to write this
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack){
    __asm volatile("MSR MSP, %0" :: "r" (sched_top_of_stack) : );
    __asm volatile("BX LR");  // this copies the value of LR into PC

}

void init_task_stack(void){
    uint32_t *pPSP;

    for (int i = 0; i < MAX_TASKS; i++){
        pPSP = (uint32_t *) psp_of_tasks[i];
        // stack model is full descending so it decrement first then stores the value
        pPSP--;
        *pPSP = DUMMY_XPSR; // 0x01000000U

        pPSP--;  // PC
        *pPSP = task_handlers[i];

        pPSP--;  // LR
        *pPSP = 0xFFFFFFFD;

        for (int j = 0; j < 13; j++){
            pPSP--;
            *pPSP = 0;
        }

        psp_of_tasks[i] = (uint32_t) pPSP; // preserve the value of PSP
        
    }
    
}
