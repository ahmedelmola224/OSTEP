# xv6 Lottery Scheduler

## Overview

This project implements a lottery scheduler within the xv6 operating system. The lottery scheduler allocates CPU time to processes based on a probabilistic method, where each process is assigned a number of tickets. The more tickets a process has, the higher its chances of being selected to run during a time slice. This approach is designed to give processes CPU time proportional to the number of tickets they hold.

## Objectives

- Gain in-depth knowledge of the xv6 kernel.
- Understand and modify the scheduling algorithm.
- Implement and integrate a new scheduling algorithm (lottery scheduler) into xv6.
- Create a graph to visually demonstrate the scheduler's effectiveness.

## Implementation Details

### System Calls

Two new system calls were introduced:

1. **`int settickets(int number)`**:
   - Sets the number of tickets for the calling process.
   - Default: Each process starts with 1 ticket.
   - Returns `0` if successful, `-1` for invalid inputs (e.g., a ticket number less than one).

2. **`int getpinfo(struct pstat *)`**:
   - Provides information about all running processes, including how many times each process was chosen to run and the process ID (PID).
   - Follows the structure defined in `pstat.h`.

### Process Inheritance

When a process is created using `fork()`, the child process inherits the same number of tickets as its parent. For example, if a parent has 10 tickets, the child process will also receive 10 tickets.

### Random Number Generation

A simple pseudo-random number generator was included in the kernel to facilitate the random selection process required by the lottery scheduler.

### pstat Structure

The `pstat` structure, defined in `pstat.h`, holds the following information:

```c
struct pstat {
  int inuse[NPROC];   // whether this slot of the process table is in use (1 or 0)
  int tickets[NPROC]; // the number of tickets this process has
  int pid[NPROC];     // the PID of each process 
  int ticks[NPROC];   // the number of ticks each process has accumulated 
};
```
This structure is filled in the kernel and passed to user space via the getpinfo system call.

### Scheduler Modification
The scheduler code was modified in proc.c to implement the lottery mechanism. The code was adjusted to select a process to run based on the number of tickets each process holds.

### Security Considerations
Careful attention was given to handling pointers passed from user space, following existing system calls (e.g., read() and sys_read()) to ensure security and correctness.

### Graphical Analysis
A graph was created to demonstrate the scheduler's behavior. The graph plots the number of time slices received by three processes with a 3:2:1 ticket ratio (e.g., Process A: 30 tickets, Process B: 20 tickets, Process C: 10 tickets). This graph illustrates that the processes receive CPU time in proportion to their ticket count.
![Lottery Scheduler Graph](images/scheduler_graph.png)

### How to Run
1. Build xv6 with the modified scheduler.
2. Use the settickets system call to assign tickets to processes.
3. Use the getpinfo system call to gather process information.
4. Compile and run the ps variant to display running process information.
5. Generate and analyze the graph to verify the lottery scheduler's behavior.

### Conclusion
This project successfully implemented a lottery scheduler in xv6, providing a new scheduling mechanism that allocates CPU time based on a lottery system. The results demonstrate that the scheduler works as intended, with processes receiving CPU time in proportion to their assigned tickets.
