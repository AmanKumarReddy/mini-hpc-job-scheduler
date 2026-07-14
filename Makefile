CC = gcc

CFLAGS = -Wall -Wextra #Wall gives warnings along with the result like when the contains Unused 
#variables; Wextra restrics some more mistakes(not errors) as Warnings in result;

TARGET = scheduler

SRC = src/main.c src/scheduler.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)