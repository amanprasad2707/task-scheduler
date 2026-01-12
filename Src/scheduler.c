#include "main.h"
#include "scheduler.h"
/* denotes the current task which is running in the CPU */
uint8_t current_task = 1; // task 1 is running
uint32_t g_tick_count = 0;

typedef struct{
    uint32_t psp_value;
    uint32_t block_count;
    uint8_t current_state;
    void (* task_handler)(void);    // function pointer to hold the task handler address
}TCB_t;

TCB_t user_tasks[MAX_TASKS];


/* ------------------------------------------------------------
 * Tick handling
 * ------------------------------------------------------------ */

void update_global_tick_count(void){
    g_tick_count++;
}


void unblock_tasks(void){
    /* Skip task 0 (idle task) */
    for (int i = 1; i < MAX_TASKS; i++){

        /* Only blocked tasks can be unblocked */
        if (user_tasks[i].current_state != TASK_STATE_BLOCKED){
            continue;
        }

        /* Check if the delay period has expired (overflow save) */
        if ((int32_t)(g_tick_count - user_tasks[i].block_count) >= 0){
            user_tasks[i].current_state = TASK_STATE_READY;
        }
    }
}



/* ------------------------------------------------------------
 * SysTick ISR
 * ------------------------------------------------------------ */

void SysTick_Handler(void){
    update_global_tick_count();
    unblock_tasks();

    schedule();
}

/* ------------------------------------------------------------
 * PendSV ISR (context switch)
 * ------------------------------------------------------------ */

/*
 * PendSV_Handler
 * ---------------
 * Performs a context switch between tasks.
 *
 * PendSV runs in handler mode using MSP, while tasks run in thread mode using PSP.
 * This handler saves the context of the current task, selects the next task to run,
 * restores its context and returns to thread mode.
 */
__attribute__((naked)) void PendSV_Handler(void){
    __asm volatile(
        /* ------------------------------------------------------------
         * Step 1: Save context of the currently running task (PSP)
         * ------------------------------------------------------------ */

        "MRS   R0, PSP        \n"   // store current running task PSP value into R0
        "STMDB R0!, {R4-R11}  \n"   // store R4-R11 register onto the stack
        /*
         * Save callee-saved registers R4–R11 onto the task stack.
         * STMDB (Decrement Before) creates space first, then stores.
         *
         * Effect (conceptual):
         *   R0 = R0 - 32;              // 8 registers × 4 bytes
         *   [R0 + 0]  = R4;
         *   [R0 + 4]  = R5;
         *   ...
         *   [R0 + 28] = R11;
         */

        "MSR   PSP, R0        \n"   // update PSP to new top of stack

        /* ------------------------------------------------------------
         * Step 2: Call scheduler functions (use MSP)
         * ------------------------------------------------------------ */

        /*
         * Protect LR (contains EXC_RETURN) and ensure a valid stack
         * for C function calls. PUSH/POP operate on MSP in handler mode.
         */
        "PUSH  {LR}           \n"

        /* Save PSP of current task, select next task to run and get its PSP */
        "BL    save_psp_value \n"
        "BL    update_next_task \n"
        "BL    get_psp_value  \n"

        /* Restore EXC_RETURN value into LR */
        "POP   {LR}           \n"

        /*
         * Restore callee-saved registers R4–R11 from the next task's stack.
         * LDMIA (Increment After) reverses the earlier STMDB.
         */
        "LDMIA R0!, {R4-R11}  \n"

        /* Update PSP to point above the restored context */
        "MSR   PSP, R0        \n"

        /* ------------------------------------------------------------
         * Step 4: Exception return
         * ------------------------------------------------------------ */

        /*
         * BX LR triggers exception return using EXC_RETURN.
         * The CPU automatically restores:
         *   R0–R3, R12, LR, PC, xPSR
         * and resumes execution of the selected task in thread mode.
         */
        "BX    LR             \n"
    );
}


/* ------------------------------------------------------------
 * Scheduler helpers
 * ------------------------------------------------------------ */


void save_psp_value(uint32_t current_psp_val){
    user_tasks[current_task].psp_value = current_psp_val;
}

void update_next_task(void){
    int state = TASK_STATE_BLOCKED;

    for (int i = 0; i < MAX_TASKS; i++){
        current_task++;
        current_task %= MAX_TASKS;
        state = user_tasks[current_task].current_state;

        if((state == TASK_STATE_READY) && (current_task != 0)){
            break;
        }
    }

    if(state != TASK_STATE_READY){
        current_task = 0;
    }
    
}


uint32_t get_psp_value(void){

    return user_tasks[current_task].psp_value;
}

void schedule(void){
    /* Request PendSV for context switching */
    SCB_ICSR = SCB_ICSR_PENDSVSET;
}


/* ------------------------------------------------------------
 * Task delay service
 * ------------------------------------------------------------ */


void task_delay(uint32_t tick_count){

    INTERRUPT_DISABLE();

    /*
     * We are changing shared scheduler data here.
     * If an interrupt happens in the middle of this code,
     * the scheduler may see incomplete or wrong values.
     *
     * So we temporarily disable interrupts to make sure
     * this update happens safely and completely.   */

    /*
     * block_count stores the absolute tick value at which the task should wake up.
     *
     * Overflow is handled during unblock using:
     * (int32_t)(g_tick_count - block_count) >= 0 */

    if(current_task){  // task 0 = idle task
        user_tasks[current_task].block_count = g_tick_count + tick_count;
        user_tasks[current_task].current_state = TASK_STATE_BLOCKED;
        schedule();
    }

    INTERRUPT_ENABLE();
}

/* ------------------------------------------------------------
 * Task stack initialization
 * ------------------------------------------------------------ */


void init_task_stack(void){
    /* ------------------------------------------------------------
     * Step 1: Initialize task states, stack start addresses,
     *         and task entry functions
     * ------------------------------------------------------------ */

    user_tasks[0].task_handler = idle_task;
    user_tasks[0].psp_value    = IDLE_STACK_START;

    user_tasks[1].task_handler = task1_handler;
    user_tasks[1].psp_value    = T1_STACK_START;

    user_tasks[2].task_handler = task2_handler;
    user_tasks[2].psp_value    = T2_STACK_START;

    user_tasks[3].task_handler = task3_handler;
    user_tasks[3].psp_value    = T3_STACK_START;

    user_tasks[4].task_handler = task4_handler;
    user_tasks[4].psp_value    = T4_STACK_START;

    for (int i = 0; i < MAX_TASKS; i++){
        user_tasks[i].current_state = TASK_STATE_READY;
    }

    /* ------------------------------------------------------------
     * Step 2: Build initial stack frame for each task
     * Stack layout must match what the Cortex-M CPU expects on exception return.
     * ------------------------------------------------------------ */
    
    for (int i = 0; i < MAX_TASKS; i++){
        uint32_t *pPSP = (uint32_t *)user_tasks[i].psp_value;

        /* Ensure 8-byte stack alignment (ARM requirement) */
        pPSP = (uint32_t *)((uint32_t)pPSP & ~0x7);

        /* ------------------------------------------------------------
         * Hardware-stacked frame (auto-popped by CPU)
         * Order is IMPORTANT
         * ------------------------------------------------------------ */

        *(--pPSP) = DUMMY_XPSR;                              // xPSR (Thumb bit set)
        *(--pPSP) = ((uint32_t)user_tasks[i].task_handler) | 0x1; // PC (task entry, Thumb)
        *(--pPSP) = EXC_RETURN_THREAD_PSP_NOFP;              // LR (EXC_RETURN → Thread, PSP)
        *(--pPSP) = 0x00000000;                              // R12
        *(--pPSP) = 0x00000000;                              // R3
        *(--pPSP) = 0x00000000;                              // R2
        *(--pPSP) = 0x00000000;                              // R1
        *(--pPSP) = 0x00000000;                              // R0

        /* ------------------------------------------------------------
         * Software-stacked frame (saved/restored in PendSV)
         * R4–R11 (callee-saved registers)
         * ------------------------------------------------------------ */

        for (int r = 0; r < 8; r++){
            *(--pPSP) = 0x00000000;   // R4–R11
        }

        /* Save the prepared PSP back into the TCB */
        user_tasks[i].psp_value = (uint32_t)pPSP;
    }
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





__attribute__((naked)) void switch_sp_to_psp(void){
    /* ------------------------------------------------------------
     * Step 1: Preserve LR (return address to main)
     * ------------------------------------------------------------ */

        /*
         * BL instructions overwrite LR.
         * Save LR on MSP so we can return to main safely.
         */

    __asm volatile("PUSH {LR}");

    /* ------------------------------------------------------------
     * Step 2: Load PSP value of the current task
     * ------------------------------------------------------------ */
    /*
     * get_psp_value() returns the PSP of the current task in R0
     * as per ARM Procedure Call Standard (AAPCS). */
    __asm volatile("BL get_psp_value");
    

    /* Initialize PSP with the task's stack pointer */
    __asm volatile("MSR PSP, R0");

    /* ------------------------------------------------------------
     * Step 3: Restore LR
     * ------------------------------------------------------------ */
    __asm volatile("POP {LR}");
    
    /* ------------------------------------------------------------
     * Step 4: Switch thread mode stack from MSP to PSP
     * ------------------------------------------------------------ */

    /* 
     * CONTROL[1] = 1 → Use PSP in thread mode
     * CONTROL[0] = 0 → Remain in privileged mode */
    
    __asm volatile("MOV R0, #0x02");
    __asm volatile("MSR CONTROL, R0");

     /* Return to main using preserved LR */
    __asm volatile("BX LR");

}

