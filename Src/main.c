#include <stdint.h>
#include <stdio.h>

/* some stack memory calculations */
#define SIZE_TASK_STACK         1024U
#define SIZE_SCHED_STACK        1024U

#define SRAM_START              0X20000000U
#define SRAM_SIZE               ((128) * (1024))
#define SRAM_END                ((SRAM_START) + (SRAM_SIZE))

#define T1_STACK_START          SRAM_END
#define T2_STACK_START          ((SRAM_END) - (1 * SRAM_SIZE))
#define T3_STACK_START          ((SRAM_END) - (2 * SRAM_SIZE))
#define T4_STACK_START          ((SRAM_END) - (3 * SRAM_SIZE))
#define SCHED_STACK_START       ((SRAM_END) - (4 * SRAM_SIZE))


void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

int main(void){
    
    
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
