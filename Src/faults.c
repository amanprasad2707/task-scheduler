#include <stdio.h>

/* Fault Handlers */

void HardFault_Handler(void){
    printf("Exception: Hard Fault\n");
    while (1);
}

void MemManage_Handler(void){
    printf("Exception: Memory Management Fault\n");
    while (1);
}

void BusFault_Handler(void){
    printf("Exception: Bus Fault\n");
    while (1);
}

void UsageFault_Handler(void){
    printf("Exception: Usage Fault\n");
    while (1);
}
