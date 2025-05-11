CFLAGS = -Wall -Wextra -g -std=gnu99
CC = gcc

TARGET = vina 

SRCS = main.c vinaC.c lz.c fileSafety.c

all: main.o vinaC.o fileSafety.o lz.o
	$(CC) $(CFLAGS) main.o vinaC.o fileSafety.o lz.o -o $(TARGET)

lz.o: lz.h
	$(CC) $(CFLAGS) -c lz.c

fileSafety.o: fileSafety.h
	$(CC) $(CFLAGS) -c fileSafety.c

vinaC.o: vinaC.h lz.h fileSafety.h
	$(CC) $(CFLAGS) -c vinaC.c

main.o: vinaC.h 
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o $(TARGET)

