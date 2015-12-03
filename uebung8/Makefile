# Common definitions
CC = mpicc

# Compiler flags, paths and libraries
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O2 -ggdb
LFLAGS = $(CFLAGS)
LIBS   = -lm

OBJS = jacobi-mpi.o askparams.o
OBJSSEQ = partdiff-seq.o askparams-seq.o

# Rule to create *.o from *.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

# Targets ...
all: jacobi-mpi partdiff-seq

jacobi-mpi: $(OBJS) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(LIBS)
	
partdiff-seq: $(OBJSSEQ) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSEQ) $(LIBS)

clean:
	$(RM) *.o *~

jacobi-mpi.o: jacobi-mpi.c Makefile

partdiff-seq.o: partdiff-seq.c Makefile

askparams.o: askparams.c Makefile

askparams-seq.o: askparams-seq.c Makefile
