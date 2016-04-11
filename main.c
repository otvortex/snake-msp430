#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void printArray();
void generateInitial( void );
void runSnake( void );
void incrementUp( void );
void incrementRight( void );
void incrementDown( void );
void incrementLeft( void );
int checkBoundaryUp( void );
int checkBoundaryRight( void );
int checkBoundaryDown( void );
int checkBoundaryLeft( void );

void addToLength( void );

void updateDir( void );

void move( void );

int eatUp( void );
int eat( void );

void deleteTail( void );

void createNewApple();

unsigned int snakeLength;
unsigned int directionArr[64] = {0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0};
unsigned int state = 0; //if 0, game paused; if 1 game running;
unsigned int dir; //1 = up, 2 = right, 3 = down, 4 = left
unsigned int headX; //x coordinate of head position
unsigned int headY; //y coordinate of head postion
unsigned int tailX; //x coordinate of tail position
unsigned int tailY; //y coordinate of tail position
unsigned int dirTail; //current direction of the tail movement

unsigned int arr[8][8] = {0, 0, 0, 0, 0, 0, 0, 0, 
						  0, 0, 0, 0, 0, 0, 0, 0,
						  0, 0, 0, 0, 0, 0, 0, 0,
						  0, 0, 0, 0, 0, 0, 0, 0,
						  0, 0, 0, 0, 0, 0, 0, 0,
						  0, 0, 0, 0, 0, 0, 0, 0,
						  0, 0, 0, 0, 0, 0, 0, 0,
						  0, 0, 0, 0, 0, 0, 0, 0};

int main(){
	srand(time(NULL));
	state = 1;
	snakeLength = 1;
	generateInitial();
	int t; 
	arr[5][5] = 4;
	printArray();
 	while(state) {
		//printArray();
		//char c = getchar();
		int c;
		scanf("%d", &c);
		dir = c;
		updateDir();
		runSnake();
		if(state) {
			printArray();
		}
		//sleep(1);
		
	}

}

void updateDir() {
	int i;
	if(snakeLength > 1) {
		for(i = snakeLength-2; i >= 0; i--) {
			directionArr[i+1]=directionArr[i];
		}
	}
	directionArr[0] = dir;

	printf("directions vector: ");
	for(i=0; i < 8; i++)
	{
		printf("%d", directionArr[i]);
		printf(" ");
	}
	printf("\n snakeLength is: ");
	printf("%d", snakeLength);
}

//Print the main game array in terminal
void printArray() {
	int i;
	int j;
	for( i = 0; i < 8; i++) {
		for( j = 0; j < 8; j++) {
			printf("%d ",arr[j][i]);
		}
		printf("\n");
	}
	printf("---------------\n");
}

//Generates the initial coordinates of the snake head 
void generateInitial() {
	headX = rand() % 8;
	headY = rand() % 8;
	tailX = headX;
	tailY = headY;
	if(arr[headX][headY] == 0) {
		arr[headX][headY] = 2;
	}
	else {
		generateInitial();
	}
}

//Main run, checks boundaries, wins, increments forward
void runSnake() {
	switch(dir) {
		case 1: //up
			if(checkBoundaryUp()) {
				printf("GAME OVER\n");
				state = 0;
			}
			else 
				incrementUp();
			break;
		case 2: //right
			if(checkBoundaryRight()) {
				printf("GAME OVER\n");
				state = 0;
			}
			else 
				incrementRight();
			break;
		case 3: //down
			if(checkBoundaryDown()) {
				printf("GAME OVER\n");
				state = 0;
			}
			else 
				incrementDown();
			break;
		case 4: //left
			if(checkBoundaryLeft()) {
				printf("GAME OVER\n");
				state = 0;
			} 
			else 
				incrementLeft();
			break;
	}
}
 
void addToLength() {
	snakeLength++;
		if(snakeLength >= 3) {
			directionArr[snakeLength-1] = directionArr[snakeLength-2];
		}
}

void incrementUp() {
	if(eat()) {
		arr[headX][headY] = 1;
		headY--;
		arr[headX][headY] = 2;
		addToLength();
		//updateDir();
		createNewApple();
	}
	else {
		move();
	}
}

void incrementRight() {
	if(eat()) {
		arr[headX][headY] = 1;
		headX++;
		arr[headX][headY] = 2;
		addToLength();
		//updateDir();
		createNewApple();
	}
	else {
		move();
	}
}

void incrementDown() {
	if(eat()) {
		arr[headX][headY] = 1;
		headY++;
		arr[headX][headY] = 2;
		addToLength();
		//updateDir();
		createNewApple();
	}
	else {
		move();
	}
}

void incrementLeft() {
	if(eat()) {
		arr[headX][headY] = 1;
		headX--;
		arr[headX][headY] = 2;
		addToLength();
		//updateDir();
		createNewApple();
	}
	else {
		move();
	}
}

int checkBoundaryUp() {
	return(headY == 0);
}

int checkBoundaryRight() {
	return(headX == 7);
}

int checkBoundaryDown() {
	return(headY == 7);
}

int checkBoundaryLeft() {
	return(headX == 0);
}

int eatUp() {
	return(arr[headX][headY-1] == 4);
}

int eat() {
	switch(dir) {
		case 1:
			if(arr[headX][headY-1] == 4) {
				return 1;
			}
			else if(arr[headX][headY-1] != 0) {
				printf("GAME OVER\n");
				state = 0;
			}
			break;
		case 2:
			if(arr[headX+1][headY] == 4) {
				return 1;
			}
			else if(arr[headX+1][headY] != 0) {
				printf("GAME OVER\n");
				state = 0;
			}
			break;
		case 3:
			if(arr[headX][headY+1] == 4) {
				return 1;
			}
			else if(arr[headX][headY+1] != 0) {
				printf("GAME OVER\n");
				state = 0;
			}
			break;
		case 4:
			if(arr[headX-1][headY] == 4) {
				return 1;
			}
			else if(arr[headX-1][headY] != 0) {
				printf("GAME OVER\n");
				state = 0;
			}
			break;
	}
	return 0;
}

void move() {
	switch(dir) {
		case 1:
			arr[headX][headY] = 1;
			headY--;
			arr[headX][headY] = 2;
			break;
		case 2:
			arr[headX][headY] = 1;
			headX++;
			arr[headX][headY] = 2;
			break;
		case 3:
			arr[headX][headY] = 1;
			headY++;
			arr[headX][headY] = 2;
			break;
		case 4:
			arr[headX][headY] = 1;
			headX--;
			arr[headX][headY] = 2;
			break;
	}
	deleteTail();
}

void deleteTail() {
	switch(directionArr[snakeLength-1]) {
		case 1:
			arr[tailX][tailY] = 0;
			tailY--;
			arr[tailX][tailY] = 3;
			break;
		case 2:
			arr[tailX][tailY] = 0;
			tailX++;
			arr[tailX][tailY] = 3;
			break;
		case 3:
			arr[tailX][tailY] = 0;
			tailY++;
			arr[tailX][tailY] = 3;
			break;
		case 4:
			arr[tailX][tailY] = 0;
			tailX--;
			arr[tailX][tailY] = 3;
			break;
	}
	printf("\n");
	printf("tail coordinates: ");
	printf("%d",tailX);
	printf(", ");
	printf("%d\n",tailY);
}

void createNewApple() {
	int random, i, j;
	random = rand() % (64-snakeLength);
	for( i = 0; i < 8; i++ ) {
		for( j = 0; j < 8; j++) {
			if(!(arr[i][j])) 
				if(!(random--))
					arr[i][j] = 4;
		}
	}
}
