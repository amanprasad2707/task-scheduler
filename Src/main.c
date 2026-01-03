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


#define TICK_HZ                 1000U
#define HSI_CLK_FREQ            16000000U   // 16 MHz
#define SYSTICK_TIM_CLK         HSI_CLK_FREQ


void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void init_systick_timer(uint32_t tick_hz);

int main(void){
    
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