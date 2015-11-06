# Common definitions
CC = gcc

# Compiler flags, paths and libraries
CFLAGS = -std=c99 -pedantic -Wall -Wextra -O2 -ggdb -fopenmp
LFLAGS = $(CFLAGS)
LIBS   = -lm

OBJSSEQ = partdiff-seq.o askparams.o
OBJSOPENMP = partdiff-openmp.o askparams.o
OBJSELEMENT = partdiff-openmp-element.o askparams.o
OBJSZEILE = partdiff-openmp-zeile.o askparams.o
OBJSSPALTE = partdiff-openmp-spalte.o askparams.o

# Rule to create *.o from *.c
.c.o:
	$(CC) -c $(CFLAGS) $*.c

# Targets ...
all: partdiff-seq partdiff-openmp partdiff-openmp-element partdiff-openmp-zeile partdiff-openmp-spalte

partdiff-seq: $(OBJSSEQ) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSEQ) $(LIBS)
	
partdiff-openmp: $(OBJSOPENMP) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSOPENMP) $(LIBS)	
	
partdiff-openmp-element: $(OBJSELEMENT) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSELEMENT) $(LIBS)	
	
partdiff-openmp-zeile: $(OBJSZEILE) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSZEILE) $(LIBS)	
	
partdiff-openmp-spalte: $(OBJSSPALTE) Makefile
	$(CC) $(LFLAGS) -o $@ $(OBJSSPALTE) $(LIBS)	

clean:
	$(RM) *.o *~

partdiff-seq.o: partdiff-seq.c Makefile

partdiff-openmp.o: partdiff-openmp.c Makefile

partdiff-openmp-element.o: partdiff-openmp-element.c Makefile

partdiff-openmp-zeile.o: partdiff-openmp-zeile.c Makefile

partdiff-openmp-spalte.o: partdiff-openmp-spalte.c Makefile

askparams.o: askparams.c Makefile
