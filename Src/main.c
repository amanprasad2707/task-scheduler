#include "main.h"
#include "led.h"
#include "scheduler.h"
#include "tasks.h"


void idle_task(void *arg){
    while(1);
}


void green_task(void *arg){
    while(1){
        led_on(LED_GREEN);
        task_delay(1000);
        led_off(LED_GREEN);
        task_delay(1000);
    }
}

void red_task(void *arg){
    while(1){
        led_on(LED_RED);
        task_delay(500);
        led_off(LED_RED);
        task_delay(500);
    }
}

void blue_task(void *arg){
    while(1){
        led_on(LED_BLUE);
        task_delay(250);
        led_off(LED_BLUE);
        task_delay(250);
    }
}

void orange_task(void *arg){
    while (1){
        led_on(LED_ORANGE);
        task_delay(125);
        led_off(LED_ORANGE);
        task_delay(125);
    }
}

int main(void){

    enable_processor_faults();

    init_scheduler_stack(SCHED_STACK_START);


    led_init_all();

    task_create(idle_task, NULL, 256, TASK_PRIORITY_IDLE);
    task_create(green_task, NULL, 512, TASK_PRIORITY_MEDIUM);
    task_create(blue_task, NULL, 512, TASK_PRIORITY_MEDIUM);
    task_create(red_task, NULL, 512, TASK_PRIORITY_MEDIUM);
    task_create(orange_task, NULL, 512, TASK_PRIORITY_MEDIUM);

    init_systick_timer(TICK_HZ);

    switch_sp_to_psp();



    /* Loop forever */
	for(;;);
}


