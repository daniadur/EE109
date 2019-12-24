/********************************************
*
*  Name: Dania Duran  
*  Email: daniadur@usc.edu
*  Lab section: F 12:30
*  Assignment: Lab3
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>

#define DOT_LENGTH  350  /* Define the length of a "dot" time in msec */

void dot(void);
void dash(void);
void makeOutput(char);
char checkInput(char);

int main(void) {
	// Initialize appropriate DDR registers
	DDRD |= (1 << 2); //0x04;
	DDRB &= ~((1 << 3) | (1 << 4) | (1 << 5));

	// Initialize the LED output to 0
	PORTD &= ~(1 << 2);
	//PIND = 0;

	// Enable the pull-up resistors for the 3 button inputs 
	PORTB |= ((1 << 3) | (1 << 4) | (1 << 5));// 0x38;

	// Loop forever
	while (1) {                 

		if (checkInput(5)) {// checking if button 1 is pressed
			// generating U
			dot();
			_delay_ms(DOT_LENGTH); // gap between single character
			dot(); 
			_delay_ms(DOT_LENGTH); // gap
			dash(); 
		}
		else if (checkInput(4)) { // checking if button 2 is pressed
			// generating S
			dot();
			_delay_ms(DOT_LENGTH); // gap
			dot(); 
			_delay_ms(DOT_LENGTH); // gap
			dot();
		}
		else if (checkInput(3)) { // checking if button 3 is pressed
			// generating C
			dash();
			_delay_ms(DOT_LENGTH); // gap
			dot(); 
			_delay_ms(DOT_LENGTH); // gap
			dash();
			_delay_ms(DOT_LENGTH); // gap
			dot();
		}

		_delay_ms(100); // gap between charcters
	}

	return 0;   /* never reached */
}

void dot() {
	makeOutput(1);
	_delay_ms(DOT_LENGTH);
	makeOutput(0);
}

void dash() {
	makeOutput(1);
	_delay_ms(3*DOT_LENGTH);
	makeOutput(0);

}

void makeOutput(char value) {
	if (value == 0) {
		PORTD &= ~(1 << 2); // cleared to 0
	}
	else {
		PORTD |= (1 << 2); // set to 1
	}
}

char checkInput(char bit) {
	if ((PINB & (1 << bit)) == 0) { // led on
		return 1;
	}
	else {
		return 0;
	}	
}