#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definieren Sie ein enum cardd
// Nord = 0001, Ost = 0010, Sued = 0100, West = 1000
// Nord-Ost = 0011, Sued-Ost = 0110, Sued-West = 1100, Nord-West = 1001
typedef enum {N=1, E=2, S=4, W=8} cardd;

// Definieren Sie ein 3x3-Array namens map, das Werte vom Typ cardd enthält
cardd map[3][3];

// Die Funktion set_dir soll an Position x, y den Wert dir in das Array map eintragen
// Überprüfen Sie x und y um mögliche Arrayüberläufe zu verhindern
// Überprüfen Sie außerdem dir auf Gültigkeit
void set_dir (int x, int y, cardd dir)
{
    // 0 wird als gültige Himmelsrichtung erkannt, um z.B. nachträgliches löschen möglich zu machen
    if(x >= 0 && x < 3      // X liegt zwischen 0 und 2 
        && y >= 0 && y < 3  // Y liegt zwischen 0 und 2 
        && (dir == 0 || dir == N || dir == E || dir == S || dir == W 
            || dir == (N|E) || dir == (S|E) || dir == (N|W) || dir == (S|W))) // dir ist gültige Himmelsrichtung
    {
        map[x][y] = dir;
    }
}

char* get_dir(int x, int y) {
    if(x >= 0 && x < 3      // X liegt zwischen 0 und 2 
        && y >= 0 && y < 3)  // Y liegt zwischen 0 und 2
    {
        char *rueckgabe;
        
        // Hier werden einfach nur die Dezimalwerte der oben angegebenen
        // binären Werte abgefragt und der dazugehörige Wert wird
        // geprintet.
        switch (map[x][y]) {
            case N: rueckgabe = "N";
                    break;
            case E: rueckgabe = "E";
                    break;
            case S: rueckgabe = "S";
                    break;
            case W: rueckgabe = "W";
                    break;
            case N|E: rueckgabe = "NE";
                    break;
            case S|E: rueckgabe = "SE";
                    break;
            case N|W: rueckgabe = "NW";
                    break;
            case S|W: rueckgabe = "SW";
                    break;
            default: rueckgabe = "0";
                    break;
        }
        return rueckgabe;
    }
    return "";
}

// Die Funktion show_map soll das Array in Form einer 3x3-Matrix ausgeben
void show_map (void)
{
    int x = 0;
    int y = 0;
    for(x=0; x<3; x++) {
        for(y=0; y<3; y++) {
            // Die Leerzeichen werden nicht statisch vorgegeben sondern sind abhänging
            // von dem ersten und letzten Punkt jeder Zeile.
            char* dir = get_dir(x,y);
            printf("%s%s%s", (y == 2 ? ((strlen(dir) == 1) ? "   " : "  ") : ""), dir, (y == 0 ? ((strlen(dir) == 1) ? "   " : "  ") : ""));
        }
        // Fügt einen Absatz am Ende der Zeile ein
        printf("\n");
    }
}

int main (void)
{
	// In dieser Funktion darf nichts verändert werden!
	set_dir(0, 1, N);
	set_dir(1, 0, W);
	set_dir(1, 4, W);
	set_dir(1, 2, E);
	set_dir(2, 1, S);

	set_dir(0, 0, N|W);
	set_dir(0, 2, N|E);
	set_dir(0, 2, N|S);
	set_dir(2, 0, S|W);
	set_dir(2, 2, S|E);
	set_dir(2, 2, E|W);
	
	show_map();

	return 0;
}
