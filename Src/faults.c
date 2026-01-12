#include <stdio.h>
#include "regs.h"

void enable_processor_faults(void){
    /*
     * Enable configurable system fault handlers:
     * - Memory Management Fault
     * - Bus Fault
     * - Usage Fault
     */
    SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA | SCB_SHCSR_BUSFAULTENA | SCB_SHCSR_USGFAULTENA;

}


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
