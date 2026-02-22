# STM32 Simple Task Scheduler

## Overview
This project is a lightweight, preemptive task scheduler for the STM32F407 microcontroller (Cortex-M4). It demonstrates the fundamentals of RTOS development, focusing on context switching and basic scheduling logic without complex memory protection or dynamic allocation overhead.

## Features
- **Preemptive Multitasking**: Uses the SysTick timer to switch between tasks.
- **Scheduling Algorithms**: Supports **Round-Robin** and **Priority-based** scheduling.
- **Context Switching**: Manually saves and restores CPU registers (R4-R11) using the `PendSV` exception.
- **Dual Stack Architecture**:
  - **MSP (Main Stack Pointer)**: Used by the kernel and ISRs.
  - **PSP (Process Stack Pointer)**: Used by user tasks.
 - **Task API**: Simple functions to create tasks (`task_create`, `task_create_idle`) and delay execution (`task_delay`).
- **Debug Support**: `printf` output redirected to ITM (SWO) for debugging.

## Hardware Support
- **MCU**: STM32F407VGT6
- **Board**: STM32F4 Discovery
- **Peripherals**: SysTick (Scheduler Tick), GPIO Port D (LEDs).

## Project Structure
```text
.
├── CMakeLists.txt       # CMake build configuration
├── Inc/                 # Header files
│   ├── scheduler.h      # Scheduler API
│   ├── tasks.h          # Task creation and TCB definitions
│   └── ...
├── Src/                 # Source files
│   ├── main.c           # Entry point
│   ├── scheduler.c      # Core scheduler logic (PendSV, SysTick)
│   ├── tasks.c          # Task creation and management
│   ├── led.c            # GPIO driver for board LEDs
│   ├── faults.c         # Processor fault handlers
│   └── ...
```

## Prerequisites
- **IDE/Editor**: Visual Studio Code
- **Build Tools**: CMake, Ninja
- **Compiler**: ARM GCC Toolchain (e.g., from STM32CubeCLT or Arm GNU Toolchain)
- **Debugger**: OpenOCD or ST-Link Server
- **VS Code Extensions**:
  - CMake Tools
  - Cortex-Debug

## Building the Project
This project uses CMake. You can build it using the VS Code CMake extension or the command line.

### VS Code
1. Open the project folder in VS Code.
2. Select the build kit (GCC for ARM).
3. Run the **Build** task (`Ctrl+Shift+B`) or click "Build" in the status bar.

### Command Line
```bash
mkdir build
cd build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../cmake/gnu-tools-for-stm32.cmake ..
cmake --build .
```

## Debugging
The project is configured for debugging with VS Code using the `Cortex-Debug` extension.

1. Connect the STM32F4 Discovery board via USB (ST-LINK).
2. Go to the **Run and Debug** tab in VS Code.
3. Select **"Debug with ST-Link"** or **"Cortex-Debug OpenOCD"**.
4. Press **F5** to start debugging.

### Configuration
The `.vscode/launch.json` file contains absolute paths that must be updated to match your environment:
- **`armToolchainPath`**: Path to your ARM GCC bin folder.
- **`svdFile`**: Path to the STM32F407 SVD file (for peripheral register viewing).

## Implementation Details

### Stack Layout
The SRAM is divided to support multiple stacks:
- **MSP (Main Stack Pointer)**: Used by the Scheduler and ISRs.
- **PSP (Process Stack Pointer)**: Unique stack area for each user task (T1, T2, T3, T4, Idle).

### Context Switching
Context switching is handled by the `PendSV_Handler` in `Src/scheduler.c`.
1. **Save Context**: Pushes R4-R11 onto the current task's stack (PSP).
2. **Save PSP**: Updates the Task Control Block (TCB) with the new PSP.
3. **Select Next Task**: The scheduler selects the next READY task based on the active policy (Priority or Round-Robin).
4. **Restore Context**: Loads the new task's PSP and pops R4-R11.
5. **Return**: `BX LR` returns to Thread Mode using the new PSP.

### Usage
1. **Create Idle Task**: Call `task_create_idle()` to register the idle task.
2. **Create User Tasks**: Call `task_create()` for each user task.
3. **Start Scheduler**: Call `scheduler_start()` to start multitasking.

See `Src/main.c` for the full initialization example.
