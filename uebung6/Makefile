# Common definitions
CC = mpicc

# Compiler flags, paths and libraries
CFLAGS = -std=c99 -pedantic -Wall -Wextra -ggdb -D_XOPEN_SOURCE=500
LFLAGS = $(CFLAGS)
LIBS   = -lm

OBJS = timempi.o
OBJS2 = timempi2.o

# Rule to create *.o from *.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

# Targets ...
all: timempi timempi2

timempi: $(OBJS) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(LIBS)

timempi2: $(OBJS2) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJS2) $(LIBS)

clean:
	$(RM) *.o *~ timempi timempi2

timempi.o: timempi.c Makefile

timempi2.o: timempi2.c Makefile

