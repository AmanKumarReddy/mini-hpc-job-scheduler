CC = gcc
CFLAGS = -Wall -Wextra -g

TARGETS = start stop submit status cancel output
INSTALL_DIR = /usr/local/bin

all: $(TARGETS)

start: src/scheduler/main.c src/scheduler/scheduler.c src/submit/submit_handler.c src/status/status_handler.c src/cancel/cancel_handler.c src/output/output_handler.c src/common/ipc.c src/common/job.c src/common/queue.c
	$(CC) $(CFLAGS) -o $@ $^

stop: src/stop/stop.c src/common/ipc.c
	$(CC) $(CFLAGS) -o $@ $^

submit: src/submit/submit.c src/common/ipc.c
	$(CC) $(CFLAGS) -o $@ $^

status: src/status/status.c src/common/ipc.c
	$(CC) $(CFLAGS) -o $@ $^

cancel: src/cancel/cancel.c src/common/ipc.c
	$(CC) $(CFLAGS) -o $@ $^

output: src/output/output.c src/common/ipc.c
	$(CC) $(CFLAGS) -o $@ $^

install: all
	@echo "Installing binaries globally to $(INSTALL_DIR)..."
	sudo cp $(TARGETS) $(INSTALL_DIR)
	@echo "Installation complete!"

uninstall:
	@echo "Removing binaries from $(INSTALL_DIR)..."
	sudo rm -f $(addprefix $(INSTALL_DIR)/, $(TARGETS))
	@echo "Uninstalled successfully."

clean:
	rm -f $(TARGETS) *.o /tmp/hpc_scheduler_fifo