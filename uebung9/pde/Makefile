# Common definitions
CC = vtcc -vt:cc mpicc

# Compiler flags, paths and libraries
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O2 -ggdb -fopenmp
LFLAGS = $(CFLAGS)
LIBS   = -lm

OBJS = partdiff-par.o askparams.o
OBJSSEQ = partdiff-seq.o askparams-seq.o

# Rule to create *.o from *.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

# Targets ...
all: partdiff-par partdiff-seq 

partdiff-par: $(OBJS) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(LIBS)

partdiff-seq: $(OBJSSEQ) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSEQ) $(LIBS)

clean:
	$(RM) *.o *~ partdiff-par partdiff-seq

partdiff-par.o: partdiff-par.c Makefile

partdiff-seq.o: partdiff-seq.c Makefile

askparams-.o: askparams.c Makefile

askparams-seq.o: askparams-seq.c Makefile
