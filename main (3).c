#include <msp430g2533.h>
#include <msp430.h>

//define pins used for
#define DATA BIT0 // DS -> 1.0
#define CLOCK BIT4 // SH_CP -> 1.4
#define LATCH BIT5 // ST_CP -> 1.5
#define ENABLE BIT6 // OE -> 1.6

// The OE pin can be tied directly to ground, but controlling
// it from the MCU lets you turn off the entire array without
// zeroing the register

// Declare functions pertaining to displaying LED's
void pulseClock ( void );
void shiftOut ( unsigned char );
void enable ( void );
void disable ( void );
void init ( void );
void pinWrite ( unsigned int, unsigned char );
void latchOn( void );
void latchOff (void );
int concatenate( int[] );
void reset( void );

//Declare functions pertaining to backend code (actual game)
void generateInitial( void );		//generates first dot
void runSnake( void );				//main function to run game
void checkForApples( void );		//checks that there is one apple, and places one if not

//function that increment game forward based on direction of head of snake:
void incrementUp( void );
void incrementRight( void );
void incrementDown( void );
void incrementLeft( void );

//functions to check whether reached edge of board
int checkBoundaryUp( void );
int checkBoundaryRight( void );
int checkBoundaryDown( void );
int checkBoundaryLeft( void );

void takeTurn( void );
void convertArrays( void );		//converts array containing game to array that will be displayed,
								//		based on hardware configuration of LED's connected to pins
								//		of the shift registers

void addToLength( void );		//adds one to the length of the snake when eating an apple
void updateDir( void );			//updates array of directions of each piece of the snake
void move( void );				//moves snake forward on pixel
int eat( void );				//eats the next apple if it encounters on
void deleteTail( void );		//deletes tail of snake after its moved forward one
void createNewApple( void );			//creates new random apple
void showScore( void );					//diasplays score on "arr"

//initialize array of directions of snake pieces
volatile unsigned int directionArr[64] = {0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0};

volatile unsigned int snakeLength;		//current length of snake
volatile unsigned int state = 0; 		//if 0, game needs to be reset; if 1 game running; if 2 game is paused until press of any button, if 3: displaying previous score
volatile unsigned int dir; 				//1 = up, 2 = right, 3 = down, 4 = left
volatile unsigned int headX; 			//x coordinate of head position
volatile unsigned int headY; 			//y coordinate of head postion
volatile unsigned int tailX; 			//x coordinate of tail position
volatile unsigned int tailY; 			//y coordinate of tail position
volatile unsigned int dirTail; 			//current direction of the tail movement


volatile unsigned int display_counter;	//interval of timer for refreshing display
unsigned int timerCount = 16;

//internal debounce "button_pressed" variables for 4 buttons
volatile unsigned int button_1_pressed = 0;
volatile unsigned int button_2_pressed = 0;
volatile unsigned int button_3_pressed = 0;
volatile unsigned int button_4_pressed = 0;

//initialize array containing actual game info:
volatile unsigned int arr[8][8] = {0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0,
								   0, 0, 0, 0, 0, 0, 0, 0};

//initialize array displaying game:
volatile unsigned int arrToPrint[8][8] = {{1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1},
   	     	     	     	    		  {1, 1, 1, 1, 1, 1, 1, 1}};

volatile unsigned int randomX;
volatile unsigned int randomY;
volatile unsigned int randomInt = 0;

//convers arrays from "arr" containing game info to hardware-based "arrToPrint", to display it on LED's
void convertArrays( void ) {
	arrToPrint[1][7] = arr[0][0];
	arrToPrint[7][0] = arr[0][1];
	arrToPrint[7][2] = arr[0][2];
	arrToPrint[7][1] = arr[0][3];
	arrToPrint[2][7] = arr[0][4];
	arrToPrint[4][0] = arr[0][5];
	arrToPrint[4][2] = arr[0][6];
	arrToPrint[4][1] = arr[0][7];
	arrToPrint[1][5] = arr[1][0];
	arrToPrint[1][6] = arr[1][1];
	arrToPrint[7][4] = arr[1][2];
	arrToPrint[7][3] = arr[1][3];
	arrToPrint[2][5] = arr[1][4];
	arrToPrint[2][6] = arr[1][5];
	arrToPrint[4][4] = arr[1][6];
	arrToPrint[4][3] = arr[1][7];
	arrToPrint[1][3] = arr[2][0];
	arrToPrint[1][4] = arr[2][1];
	arrToPrint[7][6] = arr[2][2];
	arrToPrint[7][5] = arr[2][3];
	arrToPrint[2][3] = arr[2][4];
	arrToPrint[2][4] = arr[2][5];
	arrToPrint[4][6] = arr[2][6];
	arrToPrint[4][5] = arr[2][7];
	arrToPrint[1][1] = arr[3][0];
	arrToPrint[1][2] = arr[3][1];
	arrToPrint[6][0] = arr[3][2];
	arrToPrint[7][7] = arr[3][3];
	arrToPrint[2][1] = arr[3][4];
	arrToPrint[2][2] = arr[3][5];
	arrToPrint[5][0] = arr[3][6];
	arrToPrint[4][7] = arr[3][7];
	arrToPrint[0][7] = arr[4][0];
	arrToPrint[1][0] = arr[4][1];
	arrToPrint[6][2] = arr[4][2];
	arrToPrint[6][1] = arr[4][3];
	arrToPrint[3][7] = arr[4][4];
	arrToPrint[2][0] = arr[4][5];
	arrToPrint[5][2] = arr[4][6];
	arrToPrint[5][1] = arr[4][7];
	arrToPrint[0][5] = arr[5][0];
	arrToPrint[0][6] = arr[5][1];
	arrToPrint[6][4] = arr[5][2];
	arrToPrint[6][3] = arr[5][3];
	arrToPrint[3][5] = arr[5][4];
	arrToPrint[3][6] = arr[5][5];
	arrToPrint[5][4] = arr[5][6];
	arrToPrint[5][3] = arr[5][7];
	arrToPrint[0][3] = arr[6][0];
	arrToPrint[0][4] = arr[6][1];
	arrToPrint[6][6] = arr[6][2];
	arrToPrint[6][5] = arr[6][3];
	arrToPrint[3][3] = arr[6][4];
	arrToPrint[3][4] = arr[6][5];
	arrToPrint[5][6] = arr[6][6];
	arrToPrint[5][5] = arr[6][7];
	arrToPrint[0][1] = arr[7][0];
	arrToPrint[0][2] = arr[7][1];
	arrToPrint[0][0] = arr[7][2];
	arrToPrint[6][7] = arr[7][3];
	arrToPrint[3][1] = arr[7][4];
	arrToPrint[3][2] = arr[7][5];
	arrToPrint[3][0] = arr[7][6];
	arrToPrint[5][7] = arr[7][7];
}

int main(){
	// setup the watchdog timer as an interval timer
	WDTCTL =(WDTPW + 	  // (bits 15-8) password
						  // bit 7=0 => watchdog timer on
						  // bit 6=0 => NMI on rising edge (not used here)
						  // bit 5=0 => RST/NMI pin does a reset (not used here)

				WDTTMSEL + 		// (bit 4) select interval timer mode
				WDTCNTCL +  	// (bit 3) clear watchdog timer counter
						0 		// bit 2=0 => SMCLK is the source
						+1 		// bits 1-0 = 01 => source/8K
			);
	IE1 |= WDTIE;	// enable the WDT interrupt (in the system interrupt register IE1)

	CCTL0 = CCIE;
	TACTL = TASSEL_2 + MC_2; // Set the timer A to SMCLCK, Continuous
	__enable_interrupt();	// Clear the timer and enable timer interrupt

	P1OUT=BIT1;		//set pin 1.1 as input
	P1REN=BIT1;		//enable P1.1 pull resistor
	P1OUT=BIT2;		//set pin 1.2 as input
	P1REN=BIT2;		//enable P1.2 pull resistor
	P2OUT=BIT5;		//set pin 2.5 as input
	P2REN=BIT5;   	//enable P2.5 pull resistor
	P1OUT=BIT7;		//set pin 1.7 as input
	P1REN=BIT7;		//enable P1.7 pull resistor

	P1DIR |= (DATA + CLOCK + LATCH + ENABLE);  // Setup pins as outputs
	enable(); // Enable output (pull OE low)

	//initialize interval time counter for display refresh
	display_counter = 10;

	//initialize snake length for first dot
	snakeLength = 1;

	//initialize first dot of snake
	generateInitial();

	//creates new first apple
	createNewApple();

	//set initial direction to some number 1-4
	dir = 2;

	//initialize game to state = 2, means paused
	state = 2;

	//Enter Low Power Mode 0 with interrupts enabled
	while (1)
	{
		__bis_SR_register(LPM0 + GIE);
	}
}

// ===== Watchdog Timer Interrupt Handler =====
// This event handler is called to handle the watchdog timer interrupt,
//    which is occurring regularly at intervals of about 8K/1.1MHz ~= 7.4ms.
interrupt void WDT_interval_handler(){
	if(state==0)	{				//reset game if state is 0
		reset();
	}

	randomInt++;
  if (--display_counter==0){          // decrement the counter and act only if it has reached 0
	  display_counter = 10;			//reset counter to display refresh interval

	  //covnert "arr" array, which contains contents of game, to the array that needs to be displayed
	  convertArrays();

	  //procedure to print array on board: latchOff registers, push all 8 bytes, one by one, cancatanating each, with shiftOut, then latchOn
	  int i;
	  latchOff();
	  for (i = 7; i >= 0; i--) {
		  shiftOut(concatenate(arrToPrint[i]));
	  }
	  latchOn();

  }

  //check for button pressed for each button, debounced, and generates pseudo-random X and Y based on timing of button press				1,4; 6,0;
  if(P1IN & BIT1)	{
	  randomInt++;
	  button_1_pressed = 1;
  }
  else if(button_1_pressed == 1)	{
	  randomY = randomInt%8;
	  button_1_pressed = 0;
	  dir = 1;
	  if(state == 3) {				//if displaying previous score, reset game on press
		  reset();
	  }
	  else if(state == 2) {			//if paused, start game on press
		  state = 1;
	  }
  }

  if(P1IN & BIT2)	{
	randomInt++;
    button_2_pressed = 1;
    }
  else if(button_2_pressed == 1)	{
	  randomY = randomInt%8;
	  button_2_pressed = 0;
	  dir = 2;
	  if(state == 3) {				//if displaying previous score, reset game on press
	  		  reset();
	  	  }
	  else if(state == 2) {			//if paused, start game on press
	  	  state = 1;
	  }
  }

  if(P2IN & BIT5)	{
	  randomInt++;
	  button_3_pressed = 1;
  }
  else if(button_3_pressed == 1)	{
	  randomY = randomInt%8;
	  button_3_pressed = 0;
	  dir = 3;
	  if(state == 3) {				//if displaying previous score, reset game on press
	  		  reset();
	  	  }
	  else if(state == 2) {			//if paused, start game on press
	  	  state = 1;
	  }
  }

  if(P1IN & BIT7)	{
	  randomInt++;
	  button_4_pressed = 1;
  }
  else if(button_4_pressed == 1)	{
	  randomY = randomInt%8;
	  button_4_pressed = 0;
	  dir = 4;
	  if(state == 3) {				//if displaying previous score, reset game on press
	  		  reset();
	  	  }
	  else if(state == 2) {			//if paused, start game on press
		  state = 1;
	  }
  }
}
// DECLARE function WDT_interval_handler as handler for interrupt 10
// using a macro defined in the msp430g2553.h include file
ISR_VECTOR(WDT_interval_handler, ".int10")

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	if(--timerCount ==0) {
		timerCount = 5;
		if(state == 1) {
			takeTurn();
			if(state == 1) {
				checkForApples();
			}
		}
	}
}

//increments game one turn by updating directions and running snake
void takeTurn( void ) {
	updateDir();
	runSnake();
}

//concatenates one row of "arr" for pushing it into shift registers as a byte
int concatenate(int inputArr[8]) {
	char i;
	int res = 0;
	for(i = 0; i < 8; i++) {
		res = (res << 1) + (inputArr[i] > 0);
	}
	return(res);
}

void reset( void ) {
	int i;
	int j;
	for(i = 7; i >= 0; i--) {
		for(j = 7; j >= 0; j--)	{
			arr[i][j] = 0;
		}
	}
	//initialize snake length for first dot
	snakeLength = 1;

	//initialize first dot of snake
	generateInitial();

	//creates new first apple
	createNewApple();

	//set initial direction to some number 1-4
	dir = 2;

	//initialize game to state = 2, means paused
	state = 2;
}

// Writes a value to the specified bitmask/pin. Use built in defines
// when calling this, as the shiftOut() function does.
// All nonzero values are treated as "high" and zero is "low"
void pinWrite( unsigned int bit, unsigned char val )
{
  if (val){
    P1OUT |= bit;
  } else {
    P1OUT &= ~bit;
  }
}



// Pulse the clock pin
void pulseClock( void )
{
  P1OUT |= CLOCK;
  P1OUT ^= CLOCK;
}

// set latch off to enable Shifting bits in
void latchOff( void )
{
   //Set latch to low (should be already)
   P1OUT &= ~LATCH;
}

// set latch on to set bits in output
void latchOn( void )
{
    // Pulse the latch pin to write the values into the storage register
    P1OUT |= LATCH;
    P1OUT &= ~LATCH;
}

// Take the given 8-bit value and shift it out, LSB to MSB
void shiftOut(unsigned char val)

{
  char i;
  // Iterate over each bit, set data pin, and pulse the clock to send it
  // to the shift register
  for (i = 0; i < 8; i++)  {
      pinWrite(DATA, (val & (1 << i)));
      pulseClock();
  }
}

// These functions are just a shortcut to turn on and off the array of
// LED's when you have the enable pin tied to the MCU. Entirely optional.
void enable( void )
{
  P1OUT &= ~ENABLE;

}

void disable( void )	{
  P1OUT |= ENABLE;
}

void updateDir() {
	int i;
	if(snakeLength > 1) {
		for(i = snakeLength-2; i >= 0; i--) {
				directionArr[i+1]=directionArr[i];
		}
	}
	directionArr[0] = dir;
}



//Generates the initial coordinates of the snake head
void generateInitial() {
	headX = 3; //rand() % 8;
	headY = 3; //rand() % 8;
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
				state = 3; // state 3 = game over
				showScore();
			}
			else
				incrementUp();
			break;
		case 2: //right
			if(checkBoundaryRight()) {
				state = 3;
				showScore();
			}
			else
				incrementRight();
			break;
		case 3: //down
			if(checkBoundaryDown()) {
				state = 3;
				showScore();
			}
			else
				incrementDown();
			break;
		case 4: //left
			if(checkBoundaryLeft()) {
				state = 3;
				showScore();
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

void checkForApples( void ) {
	int i;
	int j;
	int number_of_apples;
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 8; j++) {
			if(arr[i][j]==4) {
				number_of_apples++;
			}
		}
	}
	if(number_of_apples != 1) {
		createNewApple();
	}
}

void incrementUp() {
	if(eat()) {
		arr[headX][headY] = 1;
		headY--;
		arr[headX][headY] = 2;
		addToLength();
		//createNewApple();
	}
	else {
		if(state == 1) {		//check that game is still running
			move();
		}
	}
}



void incrementRight() {
	if(eat()) {
		arr[headX][headY] = 1;
		headX++;
		arr[headX][headY] = 2;
		addToLength();
		//createNewApple();
	}
	else {
		if(state == 1) {		//check that game is still running
			move();
		}
	}
}



void incrementDown() {
	if(eat()) {
		arr[headX][headY] = 1;
		headY++;
		arr[headX][headY] = 2;
		addToLength();
		//createNewApple();
	}
	else {
		if(state == 1) {		//check that game is still running
			move();
		}
	}
}

void incrementLeft() {
	if(eat()) {
		arr[headX][headY] = 1;
		headX--;
		arr[headX][headY] = 2;
		addToLength();
		//createNewApple();
	}
	else {
		if(state == 1) {		//check that game is still running
			move();
		}
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

//checks for apple in direction of head
int eat() {
	switch(dir) {
		case 1:		//eats up
			if(arr[headX][headY-1] == 4) {
				return 1;
			}
			else if(arr[headX][headY-1] != 0) {
				state = 3;
				showScore();
			}
			break;
		case 2:		//eats right
			if(arr[headX+1][headY] == 4) {
				return 1;
			}
			else if(arr[headX+1][headY] != 0) {
				state = 3;
				showScore();
			}
			break;
		case 3:		//eats down
			if(arr[headX][headY+1] == 4) {
				return 1;
			}
			else if(arr[headX][headY+1] != 0) {
				state = 3;
				showScore();
			}
			break;
		case 4:		//eats left
			if(arr[headX-1][headY] == 4) {
				return 1;
			}
			else if(arr[headX-1][headY] != 0) {
				state = 3;
				showScore();
			}
			break;
	}
	return 0;
}

void showScore( void ) {
	int i;
	int j;
	for(i = 7; i >= 0; i--) {
		for(j = 7; j >= 0; j--)	{
			arr[i][j] = 0;
		}
	}
	if(snakeLength == 1) {
		arr[0][4] = 1;
		arr[1][4] = 1;
		arr[2][4] = 1;
		arr[3][4] = 1;
		arr[4][4] = 1;
		arr[5][4] = 1;
		arr[6][4] = 1;
		arr[7][4] = 1;
		arr[1][3] = 1;
		arr[2][2] = 1;
	}
	else if(snakeLength == 2) {
		arr[3][2] = 1;
		arr[2][2] = 1;
		arr[1][3] = 1;
		arr[1][4] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][4] = 1;
		arr[5][3] = 1;
		arr[6][2] = 1;
		arr[7][2] = 1;
		arr[7][3] = 1;
		arr[7][4] = 1;
		arr[7][5] = 1;
	}
	else if(snakeLength == 3) {
		arr[1][2] = 1;
		arr[1][3] = 1;
		arr[1][4] = 1;
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[4][4] = 1;
		arr[4][3] = 1;
		arr[4][2] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
		arr[7][4] = 1;
		arr[7][3] = 1;
		arr[7][2] = 1;
	}
	else if(snakeLength == 4) {
		arr[1][2] = 1;
		arr[2][2] = 1;
		arr[3][2] = 1;
		arr[4][2] = 1;
		arr[4][3] = 1;
		arr[4][4] = 1;
		arr[4][5] = 1;
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
	}
	else if(snakeLength == 5) {
		arr[1][2] = 1;
		arr[2][2] = 1;
		arr[3][2] = 1;
		arr[4][2] = 1;
		arr[4][3] = 1;
		arr[4][4] = 1;
		arr[4][5] = 1;
		arr[1][5] = 1;
		arr[1][4] = 1;
		arr[1][3] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
		arr[7][2] = 1;
		arr[7][3] = 1;
		arr[7][4] = 1;
	}
	else if(snakeLength == 6) {
		arr[1][2] = 1;
		arr[2][2] = 1;
		arr[3][2] = 1;
		arr[4][2] = 1;
		arr[4][3] = 1;
		arr[4][4] = 1;
		arr[4][5] = 1;
		arr[1][5] = 1;
		arr[1][4] = 1;
		arr[1][3] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
		arr[7][2] = 1;
		arr[7][3] = 1;
		arr[7][4] = 1;
		arr[6][2] = 1;
		arr[5][2] = 1;
	}
	else if(snakeLength == 7) {
		arr[1][2] = 1;
		arr[1][3] = 1;
		arr[1][4] = 1;
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
	}
	else if(snakeLength == 8) {
		arr[1][2] = 1;
		arr[2][2] = 1;
		arr[3][2] = 1;
		arr[4][2] = 1;
		arr[4][3] = 1;
		arr[4][4] = 1;
		arr[4][5] = 1;
		arr[1][5] = 1;
		arr[1][4] = 1;
		arr[1][3] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
		arr[7][2] = 1;
		arr[7][3] = 1;
		arr[7][4] = 1;
		arr[6][2] = 1;
		arr[5][2] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
	}
	else if(snakeLength == 9) {
		arr[1][2] = 1;
		arr[2][2] = 1;
		arr[3][2] = 1;
		arr[4][2] = 1;
		arr[4][3] = 1;
		arr[4][4] = 1;
		arr[4][5] = 1;
		arr[1][5] = 1;
		arr[1][4] = 1;
		arr[1][3] = 1;
		arr[5][5] = 1;
		arr[6][5] = 1;
		arr[7][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
	}
	else if(snakeLength == 10) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//zero on right:
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
	}
	else if(snakeLength == 11) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//1 on right:
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
	}
	else if(snakeLength == 12) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//2 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 13) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//3 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 14) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//4 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[2][5] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 15) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//5 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 16) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//6 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 17) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//7 on right:
		arr[1][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
	}
	else if(snakeLength == 18) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//8 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 19) {
		//1 on left:
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[4][3] = 1;
		arr[5][3] = 1;

		//9 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 20) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//zero on right:
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
	}
	else if(snakeLength == 21) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//1 on right:
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
	}
	else if(snakeLength == 22) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//2 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 23) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//3 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 24) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//4 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[2][5] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 25) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//5 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 26) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//6 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 27) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//7 on right:
		arr[1][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
	}
	else if(snakeLength == 28) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//8 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 29) {
		//2 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//9 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 30) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//zero on right:
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
	}
	else if(snakeLength == 31) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//1 on right:
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
	}
	else if(snakeLength == 32) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//2 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 33) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//3 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 34) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//4 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[2][5] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 35) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//5 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 36) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//6 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 37) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//7 on right:
		arr[1][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
	}
	else if(snakeLength == 38) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//8 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 39) {
		//3 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//9 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 40) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//zero on right:
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
	}
	else if(snakeLength == 41) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//1 on right:
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
	}
	else if(snakeLength == 42) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//2 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 43) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//3 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 44) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//4 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[2][5] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 45) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//5 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 46) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//6 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 47) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//7 on right:
		arr[1][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
	}
	else if(snakeLength == 48) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//8 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 49) {
		//4 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[1][3] = 1;
		arr[2][3] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[2][1] = 1;
		arr[3][2] = 1;

		//9 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 50) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//zero on right:
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
	}
	else if(snakeLength == 51) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//1 on right:
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
	}
	else if(snakeLength == 52) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//2 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 53) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//3 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 54) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//4 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[2][5] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 55) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//5 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 56) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//6 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][5] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 57) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//7 on right:
		arr[1][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
	}
	else if(snakeLength == 58) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//8 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
	}
	else if(snakeLength == 59) {
		//5 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][3] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;

		//9 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[2][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 60) {
		//6 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;
		arr[4][3] = 1;

		//zero on right:
		arr[1][5] = 1;
		arr[2][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
	}
	else if(snakeLength == 61) {
		//6 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;
		arr[4][3] = 1;

		//1 on right:
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[4][7] = 1;
		arr[5][7] = 1;
	}
	else if(snakeLength == 62) {
		//6 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;
		arr[4][3] = 1;

		//2 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][5] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 63) {
		//6 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;
		arr[4][3] = 1;

		//3 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[5][5] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[1][6] = 1;
		arr[5][6] = 1;
		arr[3][6] = 1;
	}
	else if(snakeLength == 64) {
		//6 on left:
		arr[1][1] = 1;
		arr[3][1] = 1;
		arr[4][1] = 1;
		arr[5][1] = 1;
		arr[1][3] = 1;
		arr[2][1] = 1;
		arr[3][3] = 1;
		arr[5][3] = 1;
		arr[1][2] = 1;
		arr[5][2] = 1;
		arr[3][2] = 1;
		arr[4][3] = 1;

		//4 on right:
		arr[1][5] = 1;
		arr[3][5] = 1;
		arr[4][7] = 1;
		arr[1][7] = 1;
		arr[2][7] = 1;
		arr[3][7] = 1;
		arr[5][7] = 1;
		arr[2][5] = 1;
		arr[3][6] = 1;
	}
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
}

unsigned volatile int random = 0;

void createNewApple() {
	unsigned int i, j;
	random = randomInt%64;
	for( i = 0; i < 8; i++ ) {
		for( j = 0; j < 8; j++) {
			if((arr[i][j])==0) {
				if((random--)==0)	{
					arr[i][j] = 4; // place new apple
					break;
				}

			}
		}
	}
}



