/********************************************
 *
 *  Name: dania duran
 *  Email: daniadur@usc.edu
 *  Section: 12:30 TTh
 *  Assignment: Lab 7 - Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"

// declaring as global variable
volatile unsigned int new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile int count = 0;		// Count to display
volatile unsigned char a, b, input;

void variable_delay_us(int);
void play_note(unsigned short);

// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
unsigned short frequency[8] =
    { 262, 294, 330, 349, 392, 440, 494, 523 };

int main(void) {

    // Initialize DDR and PORT registers and LCD
    DDRB |= (1 << 4);
    PORTC |= ( (1 << 1) | (1 << 5) ); // enable pull up
    PORTB &= ~(1 << 4); // set bit 4 as output
   	lcd_init();

    // interrupt initialization
   	PCICR |= (1 << PCIE1); // enable pin change interrupt
   	PCMSK1 |= ( (1 << PCINT9) | (PCINT13) ); // enable interrupt
   	sei(); // enable global interrupts

    // Write a spash screen to the LCD
    lcd_splash(7);

    // Read the A and B inputs to determine the intial state
    // Warning: Do NOT read A and B separately.  You should read BOTH inputs
    // at the same time, then determine the A and B values from that value.    
    input = PINC;
    a = (input & (1 << PC1)); // only getting a input
    b = (input & (1 << PC5)); // only taking in b input

    if (!b && !a) {// if 00
		old_state = 0;
	}
    else if (!b && a) {// 01
		old_state = 1;
    }
    else if (b && a) {// 11
		old_state = 2;
	}
    else if (b && !a) {// 10
		old_state = 3;
    }

    new_state = old_state;

    // Display count 0
    char buf[3];
    snprintf(buf, 3, "%d", count);
    lcd_moveto(0, 14);
    lcd_stringout(buf);
    
    while (1) {                 // Loop forever
	
	    if (changed) { // Did state change?
		    changed = 0;        // Reset changed flag

			// Output count to LCD

			// Not sure if we have to make the count go back to 0
			// but if we do heres the code to do it
			/*
			if ( (count > 64) || (count < -64) ) {
				count = 0;
			}
			*/
			char print[3];
		    snprintf(print, 3, "%d", count);
		    lcd_moveto(0, 14);
		    lcd_stringout(print);

		    // Do we play a note?
		    if ((count % 8) == 0) {
				// Determine which note (0-7) to play
			    unsigned int note = (abs(count)%64)/8;
			    // Call play_note and pass it the frequency
				play_note((unsigned short)(frequency[note]));
		    }			
	    }
    }
}

 // Play a tone at the frequency specified for one second
void play_note(unsigned short freq) {
    unsigned long period;
    period = 1000000 / freq;      // Period of note in microseconds

    while (freq--) {
    	PORTB |= (1 << PB4); // Make PB4 high
    	variable_delay_us(period/2); // Use variable_delay_us to delay for half the period
		PORTB &= ~(1 << PB4); // Make PB4 low
		variable_delay_us(period/2); // Delay for half the period again
    }
}

// variable_delay_us - Delay a variable number of microseconds
void variable_delay_us(int delay) {
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}

ISR(PCINT1_vect) {
// Read the input bits and determine A and B
    input = PINC;
    a = (input & (1 << PC1)); // only getting a input
    b = (input & (1 << PC5)); // only taking in b input

	// For each state, examine the two input bits to see if state
	// has changed, and if so set "new_state" to the new state,
	// and adjust the count value.

	if (old_state == 0) { //00
		if (a != 0) { // 01
			new_state = 1;
			count++;
		}
		else if (b != 0) {
			new_state = 3;
			count--;
		}
	}
	else if (old_state == 1) { //01
		if (a == 0) { // 00
			new_state = 0;
			count--;
		}
		else if (b != 0) { // 11
			new_state = 2;
			count++;
		}
	}
	else if (old_state == 2) { //11
		if (a == 0) { // 10
			new_state = 3;
			count++;
		}
		else if (b == 0) { // 01
			new_state = 1;
			count--;
		}

	}
	else {   // old_state = 3 // 10
		if (a != 0) { // 11
			new_state = 2;
			count--;
		}
		else if (b == 0) { //00
			new_state = 0;
			count++;
		}
	}
    
	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
	    old_state = new_state;
	    changed = 1;
	}
}