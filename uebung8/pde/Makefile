# Common definitions
CC = mpicc

# Compiler flags, paths and libraries
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O2 -ggdb -fopenmp
LFLAGS = $(CFLAGS)
LIBS   = -lm

OBJS = partdiff-par.o askparams-par.o
OBJSMP = partdiff-par-hybrid.o askparams-par.o
OBJSSEQ = partdiff-seq.o askparams-seq.o

# Rule to create *.o from *.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

# Targets ...
all: partdiff-par partdiff-seq partdiff-par-hybrid

partdiff-par: $(OBJS) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(LIBS)

partdiff-par-hybrid: $(OBJSMP) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSMP) $(LIBS)
	
partdiff-seq: $(OBJSSEQ) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSEQ) $(LIBS)

clean:
	$(RM) *.o *~ partdiff-par partdiff-par-hybrid partdiff-seq

partdiff-par.o: partdiff-par.c Makefile

partdiff-par-hybrid.o: partdiff-par-hybrid.c Makefile

partdiff-seq.o: partdiff-seq.c Makefile

askparams-par.o: askparams-par.c Makefile

askparams-seq.o: askparams-seq.c Makefile
