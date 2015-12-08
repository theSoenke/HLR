all: map pointer

map: map.o
	gcc -o map map.o
	
map.o: map.c
	gcc -c map.c
	
map.gdb: map.c
	gcc -ggdb -Wall map.c -o map

pointer: pointer.o
	gcc -o pointer pointer.o
	
pointer.o: pointer.c
	gcc -c pointer.c
	
clean:
	rm -f map
	rm -f pointer
	rm -f *.o
	rm -f *~
