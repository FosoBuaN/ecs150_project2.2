# User-Level Thread Library in C

## Overview
This project is a user-level thread library written in C as part of a university assignment. It provides basic thread management functionality including thread creation, yielding, and exiting. It also includes a custom semaphore implementation for synchronization, built on top of a preemption system using signals and timers.

The threading model is implemented using the `<ucontext.h>` API to manage context switching between threads. Preemption is achieved through a signal-based timer, allowing simulated concurrency without kernel-level threads.

## Technologies Used
- C (GCC)
- `<ucontext.h>`
- Signals & Timers (`<signal.h>`, `<sys/time.h>`)

## How to Run
1. Clone the repository.
2. Compile: `cd apps && make`
3. Run any of the `.x` test cases in `apps/` to quickly demonstrate functionality.

## Notes
Some test cases may expose known issues, particularly under preemption edge cases, which are documented for future improvement.
