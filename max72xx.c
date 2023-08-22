#include "max72xx.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

uint8_t max7219_buffer[MAX7219_BUFFER_SIZE];



void max7219_word(uint8_t addr, uint8_t data) {
	MAX7219_CS_LO();		// Set CS to LOW ____ start of transmission
	for (uint8_t n = MAX7219_SEG_NUM; n != 0; n--) { // Send multiple times for cascaded matrices
		max7219_byte(addr);	// Sending the address
		max7219_byte(data);	// Sending the data
	}
	MAX7219_CS_HI();		// Set CS to HIGH \__ end of transmission
	MAX7219_CLK_LO();		// Set CLK to LOW /
}




const uint8_t max7219_initseq[] PROGMEM = {
	0x09, 0x00,	// Decode-Mode Register, 00 = No decode
	0x0a, 0x01,	// Intensity Register, 0x00 .. 0x0f
	0x0b, 0x07,	// Scan-Limit Register, 0x07 to show all lines
	0x0c, 0x01,	// Shutdown Register, 0x01 = Normal Operation
	0x0f, 0x00,	// Display-Test Register, 0x01, 0x00 = Normal
};

void max7219_init() {
	DDRB |= (1 << MAX7219_CLK);	// Set CLK port as output
	DDRB |= (1 << MAX7219_CS);	// Set CS port as output
	DDRB |= (1 << MAX7219_DIN);	// Set DIN port as output
	for (uint8_t i = 0; i < sizeof (max7219_initseq);) {
		uint8_t addr = pgm_read_byte(&max7219_initseq[i++]);
		uint8_t data = pgm_read_byte(&max7219_initseq[i++]);
		max7219_word(addr, data);
	}
}



void max7219b_out(void) {
    uint8_t bit_mask = 0x80;
    for (uint8_t row = 8; row >= 1; row--) {
        uint8_t buffer_seg = 0;
        MAX7219_CS_LO();
        while (buffer_seg <= MAX7219_BUFFER_SIZE) {
            max7219_byte(row);
            for (uint8_t index = 8; index != 0; index--) {
                uint8_t col = max7219_buffer[buffer_seg + index - 9];
                MAX7219_CLK_LO();
                if (col & bit_mask)
                    MAX7219_DIN_HI();
                else
                    MAX7219_DIN_LO();
                MAX7219_CLK_HI();
            }
            buffer_seg += 8;
        }
        MAX7219_CS_HI();
        MAX7219_CLK_LO();
        bit_mask >>= 1;
    }
}



// void max7219b_out(void) {
//     uint8_t bit_mask = 0x80;
//     for (uint8_t row = 1; row <= 8; row++) {
//         uint8_t buffer_seg = MAX7219_BUFFER_SIZE;
//         MAX7219_CS_LO();
//         while (buffer_seg != 0) {
//             max7219_byte(row);
//             for (uint8_t index = 8; index != 0; index--) {
//                 uint8_t col = max7219_buffer[buffer_seg + index - 9];
//                 MAX7219_CLK_LO();
//                 if (col & bit_mask)
//                     MAX7219_DIN_HI();
//                 else
//                     MAX7219_DIN_LO();
//                 MAX7219_CLK_HI();
//             }
//             buffer_seg -= 8;
//         }
//         MAX7219_CS_HI();
//         MAX7219_CLK_LO();
//         bit_mask >>= 1;
//     }
// }

void max7219b_set(uint8_t x, uint8_t y) {

    //x = (MAX7219_BUFFER_SIZE) - x - 9;
    if (x < MAX7219_BUFFER_SIZE) max7219_buffer[x] |= (1 << y);
}
 
void max7219b_clr(uint8_t x, uint8_t y) {
    if (x < MAX7219_BUFFER_SIZE) max7219_buffer[x] &= ~(1 << y);
}

uint8_t max7219b_get(uint8_t x) {
	return max7219_buffer[x];
}


void max7219_byte(uint8_t data) {
    //for(uint8_t i = 0; i <= 8; i++) {
    for(uint8_t i = 8; i >= 1; i--) {
        MAX7219_CLK_LO();       // Set CLK to LOW
        if (data & 0x80)        // Mask the MSB of the data
            MAX7219_DIN_HI();   // Set DIN to HIGH
        else
            MAX7219_DIN_LO();   // Set DIN to LOW
        MAX7219_CLK_HI();       // Set CLK to HIGH
        data <<= 1;               // Shift to the left
    }
}




void max7219_row(uint8_t address, uint8_t data) {
    if (address >= 1 && address <= 8) max7219_word(address, data);
}