// TODO
CC = gcc

CFLAGS = -Wall -g

all: myprogram

myprogram: main.o functions.o
	$(CC) $(CFLAGS) main.o functions.o -o myprogram

main.o: main.c functions.h
	$(CC) $(CFLAGS) -c main.c

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c

clean:
	rm -f *.o myprogram
