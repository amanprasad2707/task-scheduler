#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "regs.h"
#include "mem_layout.h"
#include "cpu_defs.h"
#include "tasks.h"
#include "scheduler.h"
#include "led.h"



void enable_processor_faults(void);

#endif /* MAIN_H_ */
