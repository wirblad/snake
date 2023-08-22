#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdbool.h>
#include "analogRead.h"
#include "max72xx.h"
#include "uart.h"
       
#define ROWS 7 
#define COLS 15 

#define VERT_PIN 0
#define HORZ_PIN 1

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 

bool waitingForPlayerInput;         // OOOBBBBSSS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

typedef struct{    
    unsigned char X; 
    unsigned char Y; 
}Position;

typedef struct{
    Position pos[ROWS*COLS]; 
    int size;  
}Snake;

typedef enum {
    Snake_Direction_Left,    
    Snake_Direction_Right,
    Snake_Direction_Up,
    Snake_Direction_Down,
}Snake_Direction;

void drawSnake(Snake *snake){
    for(int segment = 0; segment < snake->size; segment++){
            max7219b_set(snake->pos[segment].X,snake->pos[segment].Y);
            max7219b_out();
    }
}

void clearSnake(Snake *snake){
    for(int segment = 0; segment < snake->size; segment++){
            max7219b_clr(snake->pos[segment].X,snake->pos[segment].Y);
            max7219b_out();
    }
}

Snake_Direction getNextSnakeDirection(Snake_Direction currentSnakeDirection){
    uint32_t horz = analogRead(HORZ_PIN);
  	uint32_t vert = analogRead(VERT_PIN);

    if (horz < 300) {   
            printf("RIGHT");
            waitingForPlayerInput = false;
            return Snake_Direction_Right;	
		}
    if (horz > 700) {   
            printf("LEFT");
            waitingForPlayerInput = false;
            return Snake_Direction_Left;
        }
    if (vert > 700) {   
            printf("UP");
            waitingForPlayerInput = false;
            return Snake_Direction_Up;
        }
    if (vert < 300) {   
            printf("DOWN");
            waitingForPlayerInput = false;
            return Snake_Direction_Down;
        }
        return currentSnakeDirection;
}

void moveSnake(Snake *snake,Snake_Direction direction){

    Position huvudPosition = snake->pos[0];

    if(direction == Snake_Direction_Up){
        if(huvudPosition.Y == 0) huvudPosition.Y = ROWS;
        else huvudPosition.Y--;
    }
    if(direction == Snake_Direction_Down){
        if(huvudPosition.Y == ROWS) huvudPosition.Y = 0;
        else huvudPosition.Y++;
    }
    if(direction == Snake_Direction_Left){
        if(huvudPosition.X == 0) huvudPosition.X = COLS;
        else huvudPosition.X--;
    }
    if(direction == Snake_Direction_Right){
        if(huvudPosition.X == COLS) huvudPosition.X = 0;
        else huvudPosition.X++;
    }
 
    for(int segment = snake->size - 1; segment > 0; segment--)   {
        snake->pos[segment] = snake->pos[segment-1];
    }

    snake->pos[0] = huvudPosition;
}

void initSnake(Snake *snake){
    
    snake->size = 1;
    snake->pos[0].X = 5;
    snake->pos[0].Y = 5;    
}


int main(){
    
    BIT_CLEAR(DDRC,VERT_PIN);
	BIT_CLEAR(DDRC,HORZ_PIN);
    
    init_serial();
    max7219_init();
    uint32_t oldhorz = 0;
	uint32_t oldvert = 0;
    
    Snake snake;                   
    initSnake(&snake);

    Snake_Direction currentDirection = Snake_Direction_Right;
    
    while(1){

        waitingForPlayerInput = true;
        drawSnake(&snake); // Tänd LEDDEN där den är
        
        //Läs joystick
/* 		uint32_t horz = analogRead(HORZ_PIN);
  		uint32_t vert = analogRead(VERT_PIN);
		if(horz != oldhorz)
			printf("Horz:%d\n",horz);
		oldhorz = horz;	
		if(vert != oldvert)
			printf("Vert:%d\n",vert);
		oldvert = vert; */

        while(waitingForPlayerInput){
            currentDirection = getNextSnakeDirection(currentDirection);
        }

        printf("TEST!!!!!!!!!!!");
        clearSnake(&snake); //SLÄCK LED
        moveSnake(&snake,currentDirection);
        _delay_ms(200);  
    }

    return 0;
}