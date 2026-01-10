#ifndef REGS_H
#define REGS_H

#include <stdint.h>

/* -------------------- RCC -------------------- */
#define RCC_AHB1ENR   (*(volatile uint32_t*)0x40023830U)

/* -------------------- GPIO -------------------- */
#define GPIOD_MODER   (*(volatile uint32_t*)0x40020C00U)
#define GPIOD_ODR     (*(volatile uint32_t*)0x40020C14U)

/* -------------------- SYSTICK -------------------- */
#define SYST_CSR      (*(volatile uint32_t*)0xE000E010U)
#define SYST_RVR      (*(volatile uint32_t*)0xE000E014U)
#define SYST_CVR      (*(volatile uint32_t*)0xE000E018U)

/* CSR bit definitions*/
#define SYST_CSR_ENABLE     (1U << 0) // enables the counter
#define SYST_CSR_TICKINT    (1U << 1) // enables systick exception request
#define SYST_CSR_CLKSOURCE  (1U << 2) // sets the processors clock source (not external)


/* -------------------- SCB -------------------- */
#define SCB_ICSR      (*(volatile uint32_t*)0xE000ED04U)
#define SCB_SHCSR     (*(volatile uint32_t*)0xE000ED24U)


/* ICSR bit definitions*/
#define SCB_ICSR_PENDSVSET      (1U << 28)

/* SHCSR bit definitions */
#define SCB_SHCSR_MEMFAULTENA   (1U << 16)
#define SCB_SHCSR_BUSFAULTENA   (1U << 17)
#define SCB_SHCSR_USGFAULTENA   (1U << 18)


#endif
