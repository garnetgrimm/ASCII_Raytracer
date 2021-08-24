#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define INIT_ROT 270

#define RAMP_SIZE 10
char* ramp = "@%#*+=-:. ";
//#define RAMP_SIZE 70
//char* ramp = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

char brightToAscii(float brightness) {
	if(brightness >= 1) brightness = 1;
	if(brightness < 0) brightness = 0;
	return ramp[(int)(brightness * (RAMP_SIZE - 1))];
}

struct Matrix {
	int rows;
	int cols;
	char** tiles;
} typedef Map, Screen;

struct Player {
	float x;
	float y;
	float rot;
} typedef Player;

int initMatrix(Map* map, int rows, int cols) {
	map->rows = rows;
	map->cols = cols;
	map->tiles = (char**)malloc(map->rows * sizeof(char*));
	for(int row = 0; row < rows; row++) {
		map->tiles[row] = (char*)malloc((map->cols+1) * sizeof(char));
		map->tiles[row][cols] = '\0';
	}
}

void setMatrixTile(Map* map, int row, int col, char type) {
	char** mapArr = map->tiles;
	char* rowArr = mapArr[row];
	char colVal = rowArr[col];
	map->tiles[row][col] = type;
}

char getMatrixTile(Map* map, int row, int col) {
	if(row < 0) return 0;
	if(col < 0) return 0;
	if(row >= map->rows) return 0;
	if(col >= map->cols) return 0;
	return map->tiles[row][col];
}

void printMap(Map* map) {
	for(int row = 0; row < map->rows; row++) {
		printf("%s\n", map->tiles[row]);
	}
}


int initGameFromFile(Map* map, Player* p1, char* filename) {
	FILE *fp = freopen(filename, "r", stdin );
    int c;
	char line[10];
    if( fp == NULL ) {
		return EXIT_FAILURE;
	} else {
		int rows = atoi(fgets(line, sizeof (line), fp));
		int cols = atoi(fgets(line, sizeof (line), fp));
		initMatrix(map, rows, cols);
		int row = 0;
		int col = 0;
        while( (c = fgetchar()) != EOF ) {
			if(col >= cols) { col = 0; row++; }
			if(row >= rows) row = 0;
			if(c == '\n') continue;
			if(c == '1') {
				c = ' ';
				p1->x = col;
				p1->y = row;
				p1->rot = INIT_ROT;
			}
			setMatrixTile(map, row, col, c);
			col++;
        }
        fclose(fp);
	}
}

void castRays(Map* map, Player* p1, Screen* screen) {
	float FOV = 90;
	int rotSteps = screen->cols;
	int forSteps = 100;
	float angle = p1->rot - (FOV/2);
	int rendDist = map->rows;
	float stepLength = (float)rendDist/(float)forSteps;
	for(int rotStep = 0; rotStep < rotSteps; rotStep++) {
		float rayLen = 0;
		float rayX = p1->x;
		float rayY = p1->y;
		for(int forStep = 0; forStep < forSteps; forStep++) {
			rayX += cos(angle*(M_PI/180.0))*stepLength;
			rayY += sin(angle*(M_PI/180.0))*stepLength;
			char c = getMatrixTile(map, (int)rayY, (int)rayX);
			if(c != ' ') { break; }
		}

		for(int row = 0; row < screen->rows/2; row++) {
			//TODO - row offset as well 3rd dim
			rayLen = sqrt(pow(p1->x - rayX, 2) + pow(p1->y - rayY, 2) + pow((float)row*0.1, 2));
			setMatrixTile(screen, (screen->rows/2) + row, rotStep, brightToAscii(rayLen/(float)rendDist));
			setMatrixTile(screen, (screen->rows/2) - row, rotStep, brightToAscii(rayLen/(float)rendDist));
		}
		angle += FOV / (rotSteps-1);
	}
}


char GetInput()
{
	HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
	LPDWORD length;
	INPUT_RECORD record;
	char ToReturn;
	while (record.Event.KeyEvent.bKeyDown)
	{
		ReadConsoleInput(input,&record,1,length) ;
		ToReturn= record.Event.KeyEvent.uChar.AsciiChar ;
	}
	return ToReturn;
}


int main(int argc, char *argv[])
{
	Map map;
	Player p1;
	Screen view;
	initMatrix(&view, 50, 150);
    if(initGameFromFile(&map, &p1, "level1.map") == EXIT_FAILURE) {
		printf("FAILED TO LOAD LEVEL\n");
		return EXIT_FAILURE;
	}
	float walkSpeed = 0.1;
	
	while(1) {
		if (GetKeyState(VK_LEFT) & 0x8000) p1.rot-=1;
		if (GetKeyState(VK_RIGHT) & 0x8000) p1.rot+=1;
		
		//A and D
		if (GetKeyState(0x41) & 0x8000) {
			p1.y+=cos(p1.rot*(M_PI/180.0))*walkSpeed;
			p1.x+=sin(p1.rot*(M_PI/180.0))*walkSpeed;
		}
		if (GetKeyState(0x44) & 0x8000) {
			p1.y-=cos(p1.rot*(M_PI/180.0))*walkSpeed;
			p1.x-=sin(p1.rot*(M_PI/180.0))*walkSpeed;
		}
		
		//W and S
		if (GetKeyState(0x57) & 0x8000) {
			p1.x+=cos(p1.rot*(M_PI/180.0))*walkSpeed;
			p1.y+=sin(p1.rot*(M_PI/180.0))*walkSpeed;
		}
		if (GetKeyState(0x53) & 0x8000) {
			p1.x-=cos(p1.rot*(M_PI/180.0))*walkSpeed;
			p1.y-=sin(p1.rot*(M_PI/180.0))*walkSpeed;
		}
	
		castRays(&map, &p1, &view);
		printMap(&view);
		printf("%f,%f,%f\n", p1.x, p1.y, p1.rot);
		printf("\033[%dA", view.rows+1); // Move up X lines;
		Sleep(1);
	}
	//printMap(&map);
	return EXIT_SUCCESS;
}