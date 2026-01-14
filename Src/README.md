# Round-Robin Task Scheduler

## Overview
This project is a custom implementation of a preemptive task scheduler for the STM32F407 microcontroller (Cortex-M4). It demonstrates the fundamentals of Real-Time Operating System (RTOS) development, including context switching, stack pointer manipulation (MSP vs. PSP), and SysTick-based time management.

## Features
- **Round-Robin Scheduling**: Cycles through multiple user tasks.
- **Context Switching**: Implemented in assembly using the `PendSV` exception.
- **Stack Isolation**:
  - **Kernel/Handlers**: Run on the Main Stack Pointer (MSP).
  - **User Tasks**: Run on the Process Stack Pointer (PSP).
- **Task Management**:
  - Task States: Ready, Blocked.
  - `task_delay()`: Non-blocking delay (blocks the task, yields CPU).
- **Fault Handling**: Implemented handlers for HardFault, MemManage, BusFault, and UsageFault.
- **ITM Debugging**: `printf` retargeted to ITM Port 0 for SWO debugging.

## Hardware Support
- **Target MCU**: STM32F407VGT6
- **Board**: STM32F407 Discovery Board (or compatible)
- **Peripherals Used**:
  - **SysTick**: System heartbeat and scheduler tick.
  - **GPIO (Port D)**: LEDs for visual task indication.

## Project Structure
```text
.
├── CMakeLists.txt       # CMake build configuration
├── Inc/                 # Header files
│   ├── cpu_defs.h       # CPU specific register definitions
│   ├── mem_layout.h     # SRAM stack layout definitions
│   ├── scheduler.h      # Scheduler API
│   └── ...
├── Src/                 # Source files
│   ├── main.c           # Entry point
│   ├── scheduler.c      # Core scheduler logic (PendSV, SysTick)
│   ├── tasks.c          # User task implementations
│   ├── faults.c         # Fault handlers
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
3. **Select Next Task**: Round-robin logic selects the next READY task.
4. **Restore Context**: Loads the new task's PSP and pops R4-R11.
5. **Return**: `BX LR` returns to Thread Mode using the new PSP.

### Task Configuration
Tasks are defined in `Src/tasks.c`. Currently, 4 tasks are configured to toggle LEDs (Green, Orange, Red, Blue) with different delay intervals.
