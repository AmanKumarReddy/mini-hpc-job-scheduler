HPC Job Scheduler
A lightweight, efficient C-based job scheduler designed to manage and execute tasks in a Unix-like environment.

🎓 About the Developer
I am a B.Tech student aspiring to become an AI Infrastructure Engineer. This project was developed for educational purposes to deepen my understanding of process management, daemonization, and system-level communication.

Note on Usage & Licensing: You are welcome to use, study, and modify this project for your own learning. However, please note that all copyright is owned by me. Please do not publish or distribute this code anywhere without providing proper credits.

## Project Structure

The project is modularized to separate concerns between core scheduler logic, communication protocols, and individual command handlers:

```text
mini-hpc-job-scheduler/
├── .vscode/             # Editor configuration
├── src/
│   ├── cancel/          # Logic for job cancellation
│   ├── common/          # Shared IPC, Job, and Queue structures
│   ├── output/          # Handler for retrieving job output
│   ├── scheduler/       # Core daemon engine (main.c, scheduler.c)
│   ├── status/          # Logic for checking job status
│   ├── stop/            # Logic for graceful shutdown
│   └── submit/          # Logic for job submission
├── .gitignore
├── Makefile
└── README.md


🏗️ Architecture & Design
This scheduler acts as a persistent background service:

Daemonized Process: The core engine runs in the background, managing a queue of tasks.

Communication: A FIFO (Named Pipe) facilitates communication between user commands and the scheduler.

Task Execution: The scheduler forks processes to execute commands asynchronously, tracking their PIDs, exit statuses, and execution times.

Logging: All job metadata is stored globally in /tmp/hpc_jobs.log for easy access from any directory.

🚀 How to Use
Initialization
Before running, compile and install the scheduler:

Bash
make
make install or sudo make install
Administrative Commands
Manage the background service using these commands:

sudo start: Initializes the scheduler daemon.

sudo stop: Gracefully shuts down the scheduler.

sudo status: Checks if the scheduler is currently running.

User CLI Commands
Once the scheduler is started, users can interact with it using:

submit "<command>": Submits a job to the queue. Note: If your command includes pipes (|), ampersands (&), or other shell operators, you must wrap the entire command in double quotes. A Job ID will be returned to track your progress.

output <id>: Displays the output of a specific job.

status <id>: Queries the current status of a submitted job.

cancel <id>: Terminates a specific job.

📋 Logging & Storage
Log Location: All activity is recorded in /tmp/hpc_jobs.log.

Persistence Warning: Files in /tmp/ are considered temporary. They may be cleared by the system automatically after 10 days or immediately upon system reboot.

Monitoring: Use tail -f /tmp/hpc_jobs.log to watch the scheduler logs in real-time.

⚠️ Important Precautions
Read-Only Monitoring: You may open /tmp/hpc_jobs.log to view it, but do not edit or save changes while the scheduler is running. Modifying this file will disrupt the scheduler's logging process and could lead to data loss or file corruption.

Backup: If you need to keep your job history, manually copy the log file to your home directory (cp /tmp/hpc_jobs.log ~/hpc_history.log).

✨ Why this is useful
This project provides a robust, "set-it-and-forget-it" way to run background tasks. It abstracts complex process management into a simple CLI, allowing you to trigger heavy commands, walk away, and check on them whenever you are ready.
---

