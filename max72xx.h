#ifndef __MAX72xx_H
#define __MAX72xx_H

#include <avr/io.h>

#define MAX7219_CS      PB2 // CS,  pin 4 on the MAX7219 Board
#define MAX7219_DIN     PB3 // DIN, pin 3 on the MAX7219 Board
#define MAX7219_CLK     PB5 // CLK, pin 5 on the MAX7219 Board

#define MAX7219_CS_HI()     PORTB |= (1 << MAX7219_CS)
#define MAX7219_CS_LO()     PORTB &= ~(1 << MAX7219_CS)
#define MAX7219_CLK_HI()    PORTB |= (1 << MAX7219_CLK)
#define MAX7219_CLK_LO()    PORTB &= ~(1 << MAX7219_CLK)
#define MAX7219_DIN_HI()    PORTB |= (1 << MAX7219_DIN)
#define MAX7219_DIN_LO()    PORTB &= ~(1 << MAX7219_DIN)

#define MAX7219_SEG_NUM 2 // The number of the segments.
#define MAX7219_BUFFER_SIZE MAX7219_SEG_NUM * 8 // The size of the buffer


void max7219_init();
void max7219b_set(uint8_t x, uint8_t y);
void max7219b_clr(uint8_t x, uint8_t y);
uint8_t max7219b_get(uint8_t x);
#endif