#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "led.h"



void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void enable_processor_faults(void);
__attribute__((naked)) void switch_sp_to_psp(void);

void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_task_stack(void);
void schedule(void);


void save_psp_value(uint32_t stack_addr);
void update_next_task(void);



int main(void){

    enable_processor_faults();

    init_scheduler_stack(SCHED_STACK_START);

    init_task_stack();

    led_init_all();

    init_systick_timer(TICK_HZ);

    switch_sp_to_psp();

    // this task is run with PSP as stack pointer
    task1_handler();


    /* Loop forever */
	for(;;);
}



void init_systick_timer(uint32_t tick_hz){
    uint32_t reload;

    /* Calculate reload value */
    reload = (SYSTICK_TIM_CLK / tick_hz) - 1U;

    /* Load reload value */
    SYST_RVR = reload;

    /* Clear current value register */
    SYST_CVR = 0U;

    /* Configure and start SysTick
     * - Processor clock
     * - Enable SysTick interrupt
     * - Enable SysTick counter
     */
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT  | SYST_CSR_ENABLE;
}







// This is a naked function so no prologue and epilogue. That's why we have to write this
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack){
    __asm volatile("MSR MSP, %0" :: "r" (sched_top_of_stack) : );
    __asm volatile("BX LR");  // this copies the value of LR into PC

}



void enable_processor_faults(void){
    /*
     * Enable configurable system fault handlers:
     * - Memory Management Fault
     * - Bus Fault
     * - Usage Fault
     */
    SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA | SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_USGFAULTENA;

}

__attribute__((naked)) void switch_sp_to_psp(void){
    /* ============================================================
     * Step 1: Preserve LR (return address to main)
     * ============================================================ */

        /*
         * BL instructions overwrite LR.
         * Save LR on MSP so we can return to main safely.
         */

    __asm volatile("PUSH {LR}");

    /* ============================================================
     * Step 2: Load PSP value of the current task
     * ============================================================ */
    /*
     * get_psp_value() returns the PSP of the current task in R0
     * as per ARM Procedure Call Standard (AAPCS). */
    __asm volatile("BL get_psp_value");
    

    /* Initialize PSP with the task's stack pointer */
    __asm volatile("MSR PSP, R0");

    /* ============================================================
     * Step 3: Restore LR
     * ============================================================ */
    __asm volatile("POP {LR}");
    
    /* ============================================================
     * Step 4: Switch thread mode stack from MSP to PSP
     * ============================================================ */

    /* 
     * CONTROL[1] = 1 → Use PSP in thread mode
     * CONTROL[0] = 0 → Remain in privileged mode */
    
    __asm volatile("MOV R0, #0x02");
    __asm volatile("MSR CONTROL, R0");

     /* Return to main using preserved LR */
    __asm volatile("BX LR");

}




