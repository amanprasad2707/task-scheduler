#include "cpu_defs.h"
#include "tasks.h"
#include "regs.h"
#include "sched_defs.h"
#include "scheduler.h"
/* denotes the current task which is running in the CPU */
uint8_t current_task = 1; // task 1 is running
uint32_t g_tick_count = 0;


extern TCB_t tcb_pool[MAX_TASKS];
sched_algo_t active_scheduler = SCHED_PRIORITY;


/* ------------------------------------------------------------
 *             Scheduler policy selection helpers
 * -----------------------------------------------------------
 * 
 * These functions implement task-selection logic for different
 * scheduling policies (Round-Robin, Priority).
 * They do NOT perform context switching.
 * They only select the index of the next READY task.
 * Task index 0 is reserved for the idle task and is selected
 * only if no user task is READY.
 * ------------------------------------------------------------ */

static uint8_t sched_rr_select_next_task(void);
static uint8_t sched_priority_select_next_task(void);


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
        if (tcb_pool[i].state != TASK_STATE_BLOCKED){
            continue;
        }

        /* Check if the delay period has expired (overflow safe) */
        if ((int32_t)(g_tick_count - tcb_pool[i].block_count) >= 0){
            tcb_pool[i].state = TASK_STATE_READY;
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
    tcb_pool[current_task].psp = (uint32_t *)current_psp_val;
}

void update_next_task(void){
    switch (active_scheduler)
    {
    case SCHED_RR:
        current_task = sched_rr_select_next_task();
        break;

    case SCHED_PRIORITY:
        current_task = sched_priority_select_next_task();
        break;
    
    default:
        current_task = 0;
        break;
    }
    
}


uint32_t get_psp_value(void){

    return (uint32_t)tcb_pool[current_task].psp;
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
        tcb_pool[current_task].block_count = g_tick_count + tick_count;
        tcb_pool[current_task].state = TASK_STATE_BLOCKED;
        schedule();
    }

    INTERRUPT_ENABLE();
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
    __asm volatile("MSR MSP, R0");  // sched_top_of_stack is in R0 due to AAPCS rule: first function argument → R0
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

void task_set_priority(uint8_t task, task_priority_t task_priority){
    INTERRUPT_DISABLE();
    tcb_pool[task].priority = task_priority;
    INTERRUPT_ENABLE();
}

/*
    * ---------------------------------------------------
    *               Scheduling Algorithms
    * ---------------------------------------------------
*/


/*
 * Round-robin scheduling policy:
 * Selects the next READY task in cyclic order.
 * Task 0 (idle) is selected only if no user task is READY.
 */
static uint8_t sched_rr_select_next_task(void){
    task_state_t state = TASK_STATE_BLOCKED;

    for (int i = 0; i < MAX_TASKS; i++){
        current_task++;
        current_task %= MAX_TASKS;
        state = tcb_pool[current_task].state;

        if((state == TASK_STATE_READY) && (current_task != 0)){
            return current_task;
        }
    }
    return 0; // idle task
}


static uint8_t sched_priority_select_next_task(void){
    uint8_t selected = 0; // idle task by default
    task_priority_t best_prio = TASK_PRIORITY_IDLE;

    for (int i = 1; i < MAX_TASKS; i++){
        if(tcb_pool[i].state == TASK_STATE_READY){
            if(tcb_pool[i].priority <= best_prio){
                best_prio = tcb_pool[i].priority;
                selected = i;
            }
        }
    }
    return selected;
}
