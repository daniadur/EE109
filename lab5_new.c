/********************************************
*  Name: dania duran
*  Email: daniadur@usc.edu
*  Section: 12:30 TTh
*  Assignment: Lab 5 - Combo Lock
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"
#include <stdio.h>

char isPressed(char bit) {
	if ((PINC & (1 << bit)) == 0) { // if button is pressed
		_delay_ms(5); // delay for bouncing
		while ((PINC & (1 << bit)) == 0) {
			// keep going until it stops bouncing
		}
		_delay_ms(5); // then delay
		return 1;
	}
	else { // not pressed
		return 0;
	}
}

	// defining top and bottom buffers
	char topbuf[11];
	char bottombuf[17];

int main(void) {
	lcd_init(); // Initialize the LCD
	DDRC |= ((1 << 2) | (1 << 4));// Setup DDR and PORT bits for the 2 input buttons
    PORTC |= ((1 << 2) | (1 << 4));   
    
     // write name
    lcd_writecommand(1);
    lcd_moveto(0, 3);
	lcd_stringout("dania duran");

	// write birthday
	char date[16];
	unsigned char month, day, year;
	month = 12;
	day = 4;
	year = 99;
	snprintf(date, 16, "bday = %d/%d/%d!", month, day, year);

	// center birthday
	lcd_moveto(1,1);
	lcd_stringout(date);
	_delay_ms(1000);

	// clear screen
	lcd_writecommand(1);

	// need to create a variable that keeps track of
	// state number
	int stateNum = 0;
	char status = 0;

	while (1) {
		status = 0; // assume locked until changed
    	if (isPressed(2)) { // button A is pressed
            if (state == 0) { // AA
                stateNum = 0;
            }
    		else if (state == 1) { //BA
    			stateNum = 2; // \0BA or ABA or BBA
    		}
    		
    		else if (state == 2) { // BAA
    			stateNum = 3;
    		}
            else if (state == 4) {
                stateNum = 0; // starts over
            }
    	}
    	else if (isPressed(4)) { // if button B is pressed
            status = 0; // assume locked
    		if (state == 0) { // B or BB
    			stateNum = 1;//ABB or BBB or \0BB
    		}
    		else if (state == 1) { // AB
				stateNum = 1; // stays here
			} 
            else if (state == 2) {
                stateNum = 1;
            }
            else if (state == 3) {
                stateNum = 4;
                status = 1; // WOO Unlocked !
            }
            else if (state == 4) {
                stateNum = 0; // back to the beginning
            }
    	}
        state = stateNum; // update state

        // Writing state to screen
    	lcd_moveto(0, 3);
    	snprintf(topbuf, 11, "State = S%d", stateNum);
    	lcd_stringout(topbuf);

		// pointers to string
		char* locked = "Locked  ";
		char* unlocked = "Unlocked";

        // writing status to screen
        lcd_moveto(1,0);
    	if (status == 0) { // locked
    		snprintf(bottombuf, 17, "Status: %s", locked);
    		lcd_stringout(bottombuf);
    	}
    	else { // unlocked
    		snprintf(bottombuf, 17, "Status: %s", unlocked);
    		lcd_stringout(bottombuf);
    	}  	
    }
    return 0;   /* never reached */
}