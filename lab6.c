/********************************************
 *
 *  Name: Dania duran
 *  Email:daniadur@usc.edu
 *  Section: 12:30 TTh
 *  Assignment: Lab 6 - Analog-to-digital conversion
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lcd.h"
#include "adc.h"

void rand_init(void);

int main(void) {
    // Initialize the LCD
	lcd_init();

    // Initialize the ADC
	adc_init();

    // Initialize the random number function
	rand_init();

    // Write splash screen
	lcd_splash();
        
    // Find a random initial position of the 'X'
	unsigned char pos = rand()%16;

    lcd_moveto(0, pos);
    lcd_stringout("x");

    lcd_moveto(1,0);
	char newPos = pos;
	unsigned char carat_result = adc_sample(3);
	unsigned char car = (carat_result/16);
	unsigned char newCar = car;
    int count = 0;
    // Display the 'X' on the screen


	// Display initial carat
	lcd_moveto(1, car);
	lcd_stringout("^");
    unsigned char adc_result;
    while (1) { // Loop forever
	
    	// Task 2
    	/*
    	unsigned char result;
    	result = adc_sample(3);
    	char buf[5];
    	snprintf(buf, 5, "%4d", result);

    	lcd_moveto(0,0);
    	lcd_stringout(buf);
    	*/

    	// Task 3
    	adc_result = adc_sample(0);

    	// If pressed, move 'X' to new position
    	if ( (adc_result >= 0) && (adc_result < 50) ) {  // right
    		newPos++;
    		if (newPos > 15) {
    			newPos = 15;
    		}
            lcd_moveto(0, pos);
            lcd_stringout(" ");

            lcd_moveto(0, newPos);
            lcd_stringout("x");
            pos = newPos;
            _delay_ms(100);
    	}
    	else if ( (adc_result >= 125) && (adc_result < 180) ) { // left
    		newPos--;
    		if (newPos < 1) {
    			newPos = 0;
    		}
            lcd_moveto(0, pos);
            lcd_stringout(" ");

            lcd_moveto(0, newPos);
            lcd_stringout("x");
            pos = newPos;
            _delay_ms(100);
    	} 
        
        // Do a conversion of potentiometer input
    	carat_result = adc_sample(3);
    	newCar = (carat_result/16); // generate col num

    	if (newCar != car) { // Move '^' to new position
    		lcd_moveto(1, car);
	    	lcd_stringout(" ");

	    	lcd_moveto(1, newCar);
	    	lcd_stringout("^");
	    	car = newCar;
    	}
        _delay_ms(10);

    	if (newCar == newPos) {// Check if '^' is aligned with 'X'
            count++;
    		if (count == 200) { // if passes 2 seconds
    			lcd_writecommand(1);
    			lcd_moveto(0, 6);
    			lcd_stringout("You");
    			lcd_moveto(1,6);
    			lcd_stringout("Win!");
    			while (1) {
    				// infinite loop
    			}
    		}
        }
		else {
			count = 0;
		}	
    }

    return 0;   /* never reached */
}

void rand_init() {
    int seed = 0;
    unsigned char i, j, x;

    // Build a 15-bit number from the LSBs of an ADC
    // conversion of the channels 1-5, 3 times each
    for (i = 0; i < 3; i++) {
		for (j = 1; j < 6; j++) {
		    x = adc_sample(j);
		    x &= 1;	// Get the LSB of the result
		    seed = (seed << 1) + x; // Build up the 15-bit result
		}
    }
    srand(seed);	// Seed the rand function
}