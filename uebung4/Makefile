# Common definitions
CC = gcc

# Compiler flags, paths and libraries
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O2 -ggdb -fopenmp
LFLAGS = $(CFLAGS)
LIBS   = -lm

OBJSSEQ = partdiff-seq.o askparams.o
OBJSOPENMP = partdiff-openmp.o askparams.o
OBJSELEMENT = partdiff-element.o askparams.o
OBJSZEILE = partdiff-zeile.o askparams.o
OBJSSPALTE = partdiff-spalte.o askparams.o

# Rule to create *.o from *.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

# Targets ...
all: partdiff-seq partdiff-openmp partdiff-element partdiff-zeile partdiff-spalte

partdiff-seq: $(OBJSSEQ) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSEQ) $(LIBS)
	
partdiff-openmp: $(OBJSOPENMP) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSOPENMP) $(LIBS)	
	
partdiff-element: $(OBJSELEMENT) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSELEMENT) $(LIBS)	
	
partdiff-zeile: $(OBJSZEILE) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSZEILE) $(LIBS)	
	
partdiff-spalte: $(OBJSSPALTE) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSPALTE) $(LIBS)	

clean:
	$(RM) *.o *~

partdiff-seq.o: partdiff-seq.c Makefile

partdiff-openmp.o: partdiff-openmp.c Makefile

partdiff-element.o: partdiff-element.c Makefile

partdiff-zeile.o: partdiff-zeile.c Makefile

partdiff-spalte.o: partdiff-spalte.c Makefile

askparams.o: askparams.c Makefile
