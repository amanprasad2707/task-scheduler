#ifndef CPU_DEFS_H
#define CPU_DEFS_H

#define DUMMY_XPSR  0x01000000U  // sets T-bit in EPSR


/* Interrupt control */
#define INTERRUPT_DISABLE()    do{  __asm volatile("MOV R0, #0x1"); __asm volatile("MSR PRIMASK, R0"); } while (0)
#define INTERRUPT_ENABLE()    do{  __asm volatile("MOV R0, #0x0"); __asm volatile("MSR PRIMASK, R0"); } while (0)

/* Exception return value */
#define EXC_RETURN_THREAD_PSP_NOFP   0xFFFFFFFD

#endif
