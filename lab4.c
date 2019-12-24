/********************************************
*
*  Name: dania duran
*  Email: daniadur@usc.edu
*  Section: 12:30 TTh
*  Assignment: Lab 4 - Up/Down counter on LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

int main(void) {
	lcd_init(); // Initialize the LCD
	DDRC |= ((1 << 2) | (1 << 4));// Setup DDR and PORT bits for the 2 input buttons
    PORTC |= ((1 << 2) | (1 << 4));   
    
 	// initialize variable
	char state = 1;
	char count = 0;

    while (1) {               // Loop forever   
		
		// Use state to determine actions
	    if ((PINC & (1 << 2)) == 0) {
	    	state = 1;
	    }
	    else if ((PINC & (1 << 4)) == 0) {
			state = 2;
	    }

		if (state == 1) { // count up
			count++;
			if(count > 9) {
				count = 0;
			}
			lcd_writecommand(1);
			lcd_writedata(count + 0x30);
		}
		else if (state == 2) { // count down
			count--;
			if (count < 0) {
				count = 9;
			}

			lcd_writecommand(1);
			lcd_writedata(count + 0x30);
		}
        
        // Delay before we go to the next iteration of the loop
	    _delay_ms(500);         
    }
    return 0;   /* never reached */
}