# Mini HPC Job Scheduler

A Linux-based job scheduler written in C that demonstrates the core concepts behind HPC (High Performance Computing) batch schedulers. The project focuses on queue management, Linux process execution, job monitoring, and persistent logging. It is designed as a systems programming project inspired by real-world HPC schedulers such as Slurm and PBS.

---

## Features

* Queue-based job scheduling using a linked list
* Interactive command-line interface
* Submit and execute Linux commands
* Process creation using `fork()`
* Program execution using `exec()`
* Process synchronization using `wait()`
* Process ID (PID) tracking
* Exit status monitoring
* High-precision execution time measurement
* Persistent job logging
* Multi-file modular C project
* Build automation using Makefile

---

## Project Structure

```
mini-hpc-job-scheduler/
│
├── src/
│   ├── main.c
│   ├── scheduler.c
│   └── scheduler.h
│
├── logs/
│   └── jobs.log
│
├── docs/
│
├── README.md
├── Makefile
└── .gitignore
```

---

## How It Works

1. User submits a job.
2. The scheduler stores the job in a waiting queue.
3. The scheduler executes the next available job using Linux system calls.
4. The parent process waits until execution completes.
5. Execution details such as PID, execution time, and exit status are collected.
6. Completed jobs are moved to the completed queue.
7. Job information is written to `logs/jobs.log`.

---

## Technologies Used

* C Programming
* Linux
* GCC
* Linked Lists
* Queue Data Structure
* Dynamic Memory Allocation
* Makefile
* Linux System Calls (`fork`, `exec`, `wait`)
* Process Management
* File Handling
* `gettimeofday()` for high-precision timing

---

## Build

```bash
make
```

---

## Run

```bash
./scheduler
```

or

```bash
make run
```

---

## Clean

```bash
make clean
```

---

## Current Version

**Version:** v2.0

### Implemented

* Interactive scheduler
* Waiting queue
* Completed queue
* Real Linux command execution
* PID tracking
* Exit status tracking
* High-precision execution timing
* Persistent job logging

---

## Future Enhancements

* Background scheduler daemon
* Separate job submission client
* Job status client
* Inter-process communication (IPC)
* Priority scheduling
* Round Robin scheduling
* Remote job execution
* Docker integration
* Distributed execution support
* Web-based monitoring dashboard

---

## Learning Objectives

This project was built to strengthen understanding of:

* Systems Programming
* Linux Process Management
* Scheduler Design
* Queue-Based Job Scheduling
* Operating System Concepts
* HPC Infrastructure Fundamentals

---

## License

This project is intended for educational and learning purposes.

---

## Author

Aman Kumar Reddy
B.Tech Student

Aspiring AI Infrastructure Engineer
