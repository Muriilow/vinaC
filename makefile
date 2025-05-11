CFLAGS = -Wall -Wextra -g -std=gnu99
CC = gcc

TARGET = vina 

SRCS = main.c vinaC.c lz.c fileSafety.c

all: main.o vinaC.o fileSafety.o lz.o
	$(CC) $(CFLAGS) main.o vinaC.o fileSafety.o lz.o -o $(TARGET)

vinaC.o: vinaC.c vinaC.h lz.h fileSafety.h
	$(CC) $(CFLAGS) -c vinaC.c

main.o: main.c vinaC.h 
	$(CC) $(CFLAGS) -c main.c
	
lz.o: lz.c lz.h
	$(CC) $(CFLAGS) -c lz.c

fileSafety.o: fileSafety.c fileSafety.h
	$(CC) $(CFLAGS) -c fileSafety.c

clean:
	rm -f *.o $(TARGET)

