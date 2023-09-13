#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <time.h>
#include<stdlib.h>
#include "analogRead.h"
#include "max72xx.h"
#include "uart.h"
#include "millis.h"
       
#define ROWS 7 
#define COLS 15
//#define WIN_LIMIT 10 
#define WIN_LIMIT ROWS*COLS 

#define VERT_PIN 0
#define HORZ_PIN 1

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 

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

    if (horz < 300 && (currentSnakeDirection != Snake_Direction_Left)) {   
            return Snake_Direction_Right;	
		}
    if (horz > 700 && (currentSnakeDirection != Snake_Direction_Right)) {         
            return Snake_Direction_Left;
        }
    if (vert > 700 && (currentSnakeDirection != Snake_Direction_Down)) {   
            return Snake_Direction_Up;
        }
    if (vert < 300 && (currentSnakeDirection != Snake_Direction_Up)) {   
            return Snake_Direction_Down;
        }
        return currentSnakeDirection;
}

bool checkIfSnakeHeadTouchesFood(Position *food, Position *huvud){

    if(food->X == huvud->X && food->Y == huvud->Y){
        return true;
    }
    return false;
}

bool moveSnake(Snake *snake,Snake_Direction direction){

    Position huvudPosition = snake->pos[0];

    if(direction == Snake_Direction_Up){
        if(huvudPosition.Y == 0) return false;
        else huvudPosition.Y--;
    }
    if(direction == Snake_Direction_Down){
        if(huvudPosition.Y == ROWS) return false;
        else huvudPosition.Y++;
    }
    if(direction == Snake_Direction_Left){
        if(huvudPosition.X == 0) return false;
        else huvudPosition.X--;
    }
    if(direction == Snake_Direction_Right){
        if(huvudPosition.X == COLS) return false;
        else huvudPosition.X++;
    }
 
    for(int segment = snake->size - 1; segment > 0; segment--)   {
        snake->pos[segment] = snake->pos[segment-1];
    }

    snake->pos[0] = huvudPosition;
    return true;
}

void initSnake(Snake *snake){

    snake->size = 1;
    snake->pos[0].X = (rand() % 15);
    snake->pos[0].Y = (rand() % 7);
}

bool isPositionOccupiedBySnake(Position *pos, Snake *snake){

    for(int segment = 0; segment < snake->size; segment++){
        
        if(pos->X == snake->pos[segment].X && pos->Y == snake->pos[segment].X)
            return true;
    }
    return false;
}

bool isPositionOccupiedBySnakeExceptHead(Position *pos, Snake *snake){ 

    for(int segment = 1; segment < snake->size; segment++){
        
        if(pos->X == snake->pos[segment].X && pos->Y == snake->pos[segment].Y)
            return true;
    }
    return false;
}

void setFoodPosition(Position *food, Snake *snake){

    while(true){
        food->Y = (rand() % 7);
        food->X = (rand() % 15);
        if(!isPositionOccupiedBySnake(&food,&snake))
            break;
    }
}

void drawFood(Position *food){
    max7219b_set(food->X,food->Y);
    max7219b_out();
}

void clearFood(Position *food){
    max7219b_clr(food->X,food->Y);
    max7219b_out();
}

int main(){
    
    BIT_CLEAR(DDRC,VERT_PIN);
	BIT_CLEAR(DDRC,HORZ_PIN);
    
    srand(time(NULL));
    
    init_serial();
    max7219_init();
    millis_init();
    sei();

    Snake_Direction currentDirection = Snake_Direction_Right;
    Snake snake;                   
    initSnake(&snake);
    Position food;

    volatile antalMilliSekunderSenasteBytet = 0;
    
    uint32_t start_horz = analogRead(HORZ_PIN);
  	uint32_t start_vert = analogRead(VERT_PIN);
    
    while(analogRead(HORZ_PIN) == start_horz && analogRead(VERT_PIN) == start_vert){
        printf("WAITING FOR PLAYER"); 
    }
    setFoodPosition(&food,&snake);
    drawFood(&food);
    
    while(1){

        currentDirection = getNextSnakeDirection(currentDirection);

        if( millis_get() - antalMilliSekunderSenasteBytet > 200 ){

            antalMilliSekunderSenasteBytet = millis_get();
            clearSnake(&snake);                        
            if(!moveSnake(&snake,currentDirection)){
                printf("GAME OVER WALL");
                break;
            }
            
            if(isPositionOccupiedBySnakeExceptHead(&snake.pos[0],&snake)){
                printf("GAME OVER TOUCHED SNAKE");
                break;
            }
            if(checkIfSnakeHeadTouchesFood(&food, &snake)){
                snake.size++;
                clearFood(&food);
                setFoodPosition(&food,&snake);
                drawFood(&food);
            }
            if(snake.size == WIN_LIMIT){        
                printf("YOU WIN!!!!");
                break;
            }
            drawSnake(&snake);                              
            //antalMilliSekunderSenasteBytet = millis_get();  
        }
    }
    return 0;
}