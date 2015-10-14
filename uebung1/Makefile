all: map pointer
	
pointer: map.o pointer.o
	gcc -o map map.o
	gcc -o pointer pointer.o
	
	
pointer.o: map.c pointer.c
	gcc -c map.c
	gcc -c pointer.c

clean:
	rm -f map
	rm -f pointer
	rm -f *.o
	rm -f *~
