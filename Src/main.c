#include "main.h"


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


