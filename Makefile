CC = gcc
CFLAGS = -g -Wall -Wno-int-to-void-pointer-cast

INCLUDES = -I/usr/local/include
LFLAGS=
LIBS = -lpthread

SRCS = santaclaus.c spinlock.c semaphore.c barrier.c test_spinlock.c test_semaphore.c test_barrier.c

OBJS = $(SRCS:.c=.o)

.PHONY: depend clean

all: santaclaus test_spinlock test_semaphore test_barrier

test_spinlock: test_spinlock.o spinlock.o
	$(CC) $(CFLAGS) $(INCLUDES) -o test_spinlock test_spinlock.o spinlock.o $(LFLAGS) $(LIBS)

test_semaphore: test_semaphore.o semaphore.o
	$(CC) $(CFLAGS) $(INCLUDES) -o test_semaphore test_semaphore.o semaphore.o $(LFLAGS) $(LIBS)

test_barrier: test_barrier.o semaphore.o barrier.o
	$(CC) $(CFLAGS) $(INCLUDES) -o test_barrier test_barrier.o semaphore.o barrier.o $(LFLAGS) $(LIBS)

santaclaus: santaclaus.o semaphore.o barrier.o
	$(CC) $(CFLAGS) $(INCLUDES) -o santaclaus santaclaus.o semaphore.o barrier.o $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
