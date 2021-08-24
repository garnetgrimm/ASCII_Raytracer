#include <stdio.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

#include <conio.h>

void main() {
    printf("Hello world\n");
	printf("beep boop\n");
	printf("\033[2A"); // Move up X lines;
	printf("Wacca Flocca\n");
}