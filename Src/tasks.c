#include "tasks.h"
#include "cpu_defs.h"
#include "led.h"
#include "main.h"
#include "sched_defs.h"


static uint8_t rtos_heap[RTOS_HEAP_SIZE];
static uint32_t heap_offset = 0;
TCB_t tcb_pool[MAX_TASKS];


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

static uint8_t *alloc_stack(uint32_t size_bytes){
    /* 8 byte alignmemnt */
    size_bytes = (size_bytes + 7) & ~0x7;
    if(heap_offset + size_bytes > RTOS_HEAP_SIZE){
        return NULL;
    }
    uint8_t *ptr = &rtos_heap[heap_offset];
    heap_offset += size_bytes;

    return ptr;
}


static uint32_t *build_initial_stack(uint8_t *stack_base, uint32_t stack_size, task_func_t entry, void *arg){
    uint32_t *pPSP = (uint32_t *) (stack_base + stack_size);

    /* 8-byte alignment (ABI requiremnt) */
    pPSP = (uint32_t *) ((uint32_t)pPSP & ~0x7);

    *(--pPSP) = DUMMY_XPSR;                     // xPSR
    *(--pPSP) = ((uint32_t)entry) | 1;          // PC
    *(--pPSP) = EXC_RETURN_THREAD_PSP_NOFP;     // LR (EXC_RETURN, PSP)
    *(--pPSP) = 0;                              // R12
    *(--pPSP) = 0;                              // R3
    *(--pPSP) = 0;                              // R2
    *(--pPSP) = 0;                              // R1
    *(--pPSP) = (uint32_t)arg;                  // R0 (argument)

    for (int i = 0; i < 8; i++){
        *(--pPSP) = 0;                          // R4-R11
    }

    return pPSP;
    
}


int task_create(void (*task_fn)(void *), void *arg, uint32_t stack_size_bytes, task_priority_t priority){
    if (!task_fn || stack_size_bytes < 64){
        return -1;
    }
    INTERRUPT_DISABLE();

    for (int i = 1; i < MAX_TASKS; i++){    // 0 idle task
        if(tcb_pool[i].state == TASK_STATE_UNUSED){
            uint8_t *stack = alloc_stack(stack_size_bytes);
            if(!stack){
                INTERRUPT_ENABLE();
                return -1;          // memory not allocated
            }

            TCB_t *tcb = &tcb_pool[i];
            
            tcb->stack_base = stack;
            tcb->stack_size = stack_size_bytes;
            tcb->priority = priority;
            tcb->entry = task_fn;
            tcb->arg = arg;
            tcb->state = TASK_STATE_READY;
            tcb->block_count = 0;

            tcb->psp = build_initial_stack(stack, stack_size_bytes, task_fn, arg);

            INTERRUPT_ENABLE();

            return i;       // task handle
        }
    }
    INTERRUPT_ENABLE();
    return -1;
    
}

