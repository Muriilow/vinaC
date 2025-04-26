CFLAGS = -Wall -Wextra -g -std=c99
CC = gcc

TARGET = vina 

SRCS = main.c vinaC.c

all:
	gcc main.c vinaC.c -o vina

	#$(CC) $(CFLAGS) main.c vinaC.c -o $(TARGET)

clean:
	rm -f *.o $(TARGET)

