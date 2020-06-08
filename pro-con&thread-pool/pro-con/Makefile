
CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: pro_con.o 
	$(CC) $(CFLAGS) -o example pro_con.o  $(PTHREADS)

pro_con.o: pro_con.c
	$(CC) $(CFLAGS) -c pro_con.c $(PTHREADS)


clean:
	rm -rf *.o
	rm -rf example

