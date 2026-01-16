#include "led.h"
#include "main.h"
#include "sched_defs.h"





void idle_task(void){
    while(1);
}

void task1_handler(void){
    while(1){
        // printf("this is task 1\n");
        task_set_priority(1, TASK_PRIORITY_HIGH);
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
