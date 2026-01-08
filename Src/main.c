#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "led.h"


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

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void enable_processor_faults(void);
__attribute__((naked)) void switch_sp_to_psp(void);

void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_task_stack(void);

void save_psp_value(uint32_t stack_addr);
void update_next_task(void);

void task_delay(uint32_t tick_count);

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

void idle_task(void){
    while(1);
}

void task1_handler(void){
    while(1){
        // printf("this is task 1\n");
        led_on(LED_GREEN);
        task_delay(1000);
        led_off(LED_GREEN);
        task_delay(1000);
        
    }
}

void task2_handler(void){
    while(1){
        // printf("this is task 2\n");
        led_on(LED_ORANGE);
        task_delay(500);
        led_off(LED_ORANGE);
        task_delay(500);
}

}

void task3_handler(void){
    while(1){
        // printf("this is task 3\n");
        led_on(LED_RED);
        task_delay(250);
        led_off(LED_RED);
        task_delay(250);
    }
    
}

void task4_handler(void){
    while(1){
        // printf("this is task 4\n");
        led_on(LED_BLUE);
        task_delay(125);
        led_off(LED_BLUE);
        task_delay(125);
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


__attribute__((naked)) void PendSV_Handler(void){
    __asm volatile(
        /* Save context of current task (PSP) */
        "MRS   R0, PSP        \n"   // get current running task PSP value
        "STMDB R0!, {R4-R11}  \n"   // using the PSP value to store Stack Frame 2 (R4-R11)
        "MSR   PSP, R0        \n"

        /* Protect LR and align MSP for C calls */
        "PUSH  {LR}           \n"

        /* Scheduler logic */
        "BL    save_psp_value \n"   // save the current value of PSP
        "BL    update_next_task \n" // decide the next task to run
        "BL    get_psp_value  \n"

        /* Restore LR */
        "POP   {LR}           \n"

        /* Restore context of next task */
        "LDMIA R0!, {R4-R11}  \n"   // using that PSP to retrieve SF2 (R4-R11)
        "MSR   PSP, R0        \n"   // update PSP

        /* Exception return */
        "BX    LR             \n"
    );
}
void update_global_tick_count(void){
    g_tick_count++;
}

void unblock_tasks(void){
    for(int i = 1; i < MAX_TASKS; i++){
        if(user_tasks[i].current_state != TASK_STATE_READY){
            if(user_tasks[i].block_count == g_tick_count){
                user_tasks[i].current_state = TASK_STATE_READY;
            }
        }
    }
}

void SysTick_Handler(void){
    update_global_tick_count();
    unblock_tasks();

    // pend the PendSV exception
    *pICSR |= (1 << 28);
}



// This is a naked function so no prologue and epilogue. That's why we have to write this
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack){
    __asm volatile("MSR MSP, %0" :: "r" (sched_top_of_stack) : );
    __asm volatile("BX LR");  // this copies the value of LR into PC

}

void init_task_stack(void){
    user_tasks[0].current_state = TASK_STATE_READY;
    user_tasks[1].current_state = TASK_STATE_READY;
    user_tasks[2].current_state = TASK_STATE_READY;
    user_tasks[3].current_state = TASK_STATE_READY;
    user_tasks[4].current_state = TASK_STATE_READY;

    user_tasks[0].psp_value = IDLE_STACK_START;
    user_tasks[1].psp_value = T1_STACK_START;
    user_tasks[2].psp_value = T2_STACK_START;
    user_tasks[3].psp_value = T3_STACK_START;
    user_tasks[4].psp_value = T4_STACK_START;

    user_tasks[0].task_handler = idle_task;
    user_tasks[1].task_handler = task1_handler;
    user_tasks[2].task_handler = task2_handler;
    user_tasks[3].task_handler = task3_handler;
    user_tasks[4].task_handler = task4_handler;

    uint32_t *pPSP;

    for (int i = 0; i < MAX_TASKS; i++){
        pPSP = (uint32_t *) user_tasks[i].psp_value;
        // stack model is full descending so it decrement first then stores the value
        pPSP--;
        *pPSP = DUMMY_XPSR; // 0x01000000U

        pPSP--;  // PC
        *pPSP = (uint32_t) user_tasks[i].task_handler;

        pPSP--;  // LR
        *pPSP = 0xFFFFFFFD;

        for (int j = 0; j < 13; j++){
            pPSP--;
            *pPSP = 0;
        }

        user_tasks[i].psp_value = (uint32_t) pPSP; // preserve the value of PSP
        
    }
    
}


void enable_processor_faults(void){
    // enable all configurable exceptions like usage fault, mem manage fault and bus fault
 	uint32_t *pSHCSR = (uint32_t *) 0xE000ED24;
	*pSHCSR |= (1 << 16);		// mem manage fault
	*pSHCSR |= (1 << 17);		// bus fault
	*pSHCSR |= (1 << 18);		// usage fault

	// *pSHCSR |= (0x7 << 16); // it enables all above mention three faults
}

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

__attribute__((naked)) void switch_sp_to_psp(void){
    // 1. initialize the PSP with task1 stack start address

    __asm volatile("PUSH {LR}"); // preserves LR which connects back to main()

    // get the value of PSP of current task
    __asm volatile("BL get_psp_value");
    // NOTE: shouldn't use only B that is just branch to get_psp_value we should branch with link because, we have to come back to this function. That's why we have to use BL here
    /* When it goes to this function the return value of stored in R0 according to the procedure call standards
    the initial stack address of the current task will be stored in R0
    */


    __asm volatile("MSR PSP, R0");  // initialize PSP
    __asm volatile("POP {LR}");
    /* when we use BL (branch with link) the value of LR will be corrupted
    because this function is called from main, LR is holding some value After this function it has to go back to main, that
    is done through the value of LR. Because LR is a link register. It connects back to the main. But
    here we are again moving to this function here.

    So, LR value is corrupted. That's a reason you have to push LR here, you have to save LR. Because,
    we want that later. I would push register set should be given in these brackets, that is LR.
    You got the PSP value, you came back here,
    you programmed PSP and then pop LR. We have to pop it back.
    */

    
    // 2. change SP to PSP using CONTROL register
    __asm volatile("MOV R0, #0x02");
    __asm volatile("MSR CONTROL, R0");
    __asm volatile("BX LR");

}

void schedule(void){
    // pend the PendSV exception
    *pICSR |= (1 << 28);
}

void task_delay(uint32_t tick_count){

    INTERRUPT_DISABLE();

    /*
     * We are changing shared scheduler data here.
     * If an interrupt happens in the middle of this code,
     * the scheduler may see incomplete or wrong values.
     *
     * So we temporarily disable interrupts to make sure
     * this update happens safely and completely.
     */

    if(current_task){  // task 0 = idle task
        user_tasks[current_task].block_count = g_tick_count + tick_count;
        user_tasks[current_task].current_state = TASK_STATE_BLOCKED;
        schedule();
    }

    INTERRUPT_ENABLE();
}



/* fault handlers */
void HardFault_Handler(void){
	printf("Exception: Hard Fault\n");
	while(1);
}

void MemManage_Handler(void){
	printf("Exception: Memory Management Fault\n");
	while(1);
}

void BusFault_Handler(void){
	printf("Exception: Bus Fault\n");
	while(1);
}

void UsageFault_Handler(void){
    printf("Exception: Usage Fault");
    while(1);
}
