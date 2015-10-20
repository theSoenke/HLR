/*
** simple error demonstration to demonstrate power of valgrind
** Julian M. Kunkel - 17.04.2008
*/

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

// Speicher auf dem Heap musste allokiert werden
int *
mistake1 ()
{
  int *buf = malloc(sizeof(int) * 6);
  int tmp[] = { 1, 1, 2, 3, 4, 5 };
  memcpy(buf, tmp, 6 * sizeof(int));
  return buf;
}

// sizeof(int) statt sizeof(char), 2 in buf[1] statt buf[2] speichern
int *
mistake2 ()
{
  int *buf = malloc (sizeof (int) * 4);
  buf[1] = 2;
  return buf;
}

// Speicher auf dem Heap musste allokiert werden, Variable mistake2_ nicht notwendig
int *
mistake3 ()
{
  //int mistake2_ = 0;
  int *buf = malloc(sizeof(int));
  buf[0] = 3;
  return buf;
}

/* sizeof(int) statt char, 4 in buf[0] statt buf[4] speichern, 
pointer auf buf wurde zu früh freigegeben */
int *
mistake4 ()
{
  int *buf = malloc (sizeof (int) * 4);
  buf[0] = 4;
  //free (buf); zu frühes freigeben des pointers
  return buf;
}

int
main (void)
{
  /* Modifizieren Sie die folgende Zeile nicht */
  int *p[4] = { &mistake1 ()[1], &mistake2 ()[1], mistake3 (), mistake4 () };

  printf ("1 %d\n", *p[0]);
  printf ("2 %d\n", *p[1]);
  printf ("3 %d\n", *p[2]);
  printf ("4 %d\n", *p[3]);


  /* Fügen sie hier die korrekten aufrufe von free() ein */
  free(p[0]-1);		
  free(p[1]-1);
  free(p[2]);
  free(p[3]);

  return 0;
}
