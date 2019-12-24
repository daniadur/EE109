/********************************************
 *
 *  Name: Dania Duran 
 *  Email: daniadur@usc.edu
 *  Section: 12:30 F
 *  Assignment: Final
 *
 ********************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "lcd.h"
#include "encoder.h"
#include "speedtrap.h"
#include "serial.h"

#define EEADR 100 // defines the EEPROM address
#define DIST 10 // define distance as 12cm

// initialize global variables
volatile char timeExceed = 0, buzzStart =0;
volatile unsigned int new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile unsigned char a, b, input;
volatile int count = 0;
volatile char buffer[7];
volatile char received = 0;
volatile char rBegin = 0;

int main (void) {
	// initialize timers

    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    TCNT1 = 0;
    OCR1A = 62500;

	// initialize registers
	DDRD &= ~( (1 << PD2) || (1 << PD3) ); // setting sensors as inputs
	DDRB |= (1 << PB3); // set timing LED as input
	PORTB &= ~(1 << PB3); // turn LED off
	sei();

	// Getting maxspeed from memory
	maxspeed = eeprom_read_byte( (void *) EEADR );
	if ( (maxspeed > 99) || (maxspeed < 1) ) {// check if eeprom time is not btwn 1-99
		eeprom_update_byte( (void *) EEADR, 0); // set speed to 0 if no speed
		maxspeed = 0;
	}

	// initialize helper functions
	lcd_init();
	encoder_init();
	serial_init();
	lcd_splash();

	// Variables
	unsigned int dist;
	char display = 0;
	char sensor1, sensor2, state = 0;
	int oldSpeed = 0, oldMax = 0;
	unsigned int time = 0, time1 = 0, time2 = 0;

	// START OF MF ENCODER ROUTINE
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
	
	while (1) {
		// getting sensor values
		sensor1 = (PIND & (1 << PD3)); // only getting a input
	    sensor2 = (PIND & (1 << PD2)); // only taking in b input
	    
	    if (state == 0) { 
	    	if (!sensor1) { // if sensor 1 is turned on
	    		if (timeExceed) { // reset timeExceed flag
	    			timeExceed = 0; // and clear screen
	    			lcd_writecommand(1);
	    		}
		    	state = 1;
		    	TCNT1 = 0; // clear timer
		    	TCCR1B |= ((1 << CS12) | (1 << CS10) ); // start timer
		    	PORTB |= (1 << PB3); // flash timing LED
		    	display = 0; // reset display flag
		    }
	    }
	    else if (state == 1) {
	    	if (timeExceed) { // if took too long
	    		PORTB &= ~(1 << PB3); // turn off timing LED
	    		lcd_writecommand(1);
		    	lcd_moveto(0, 1);
		    	lcd_stringout("Session Timed ");
		    	lcd_moveto(1, 6);
		    	lcd_stringout("Out !");
		    	state = 0; // reset state flag
	    	}
		    else if (!sensor2) { // if sensor 2 is turned on
		    	TCCR1B &= ~((1 << CS12) | (1 << CS10) ); // clear timer
		    	time = (TCNT1/(15625/1000)); // calculate into miliseconds

		    	PORTB &= ~(1 << PB3); // turn off timing LED
		    	state = 0; // reset state flag
		    	display = 1; // set display flag

		    	// determine speed
		    	dist = DIST*1000;
				time1 = dist/time;
				time2 = (dist/(time/10) - time1*10);
				speed = (time1*10 + time2);

				// Transmit speed
				char message[7];
				snprintf(message, 7, "<%d>", speed);// fill buffer with speed
			    serial_stringout(message); //transmits message
		    }
		}

	    // Check if Encoder moved
    	if (changed) { // Did state change?
		    changed = 0;// Reset changed flag
		    // fix constraints
			if (maxspeed > 99) {
				maxspeed = 99;
			}
			else if (maxspeed < 1) {
				maxspeed = 1;
			}
			oldMax = maxspeed;
			// update eeval
			eeprom_update_byte( (void *) EEADR, maxspeed);			
	    }

	    // display max speed
	    if (!timeExceed) {
			lcd_moveto(1, 0);
			char print1[11];
		    snprintf(print1, 11, "max:%2dcm/s", maxspeed);
		    lcd_stringout(print1);
		}

	    // display local speed
	    if (display) {
			lcd_moveto(0,0);
			char print[16];
			snprintf(print, 16, "%4dms %2d.%dcm/s", time, time1, time2);
			lcd_stringout(print);	

		}	

	    // Receiving
	    if (received) {
	    	received = 0;

	    	int rSpeed;
	    	// scan buffer for speed
	    	sscanf(buffer, "%d", &rSpeed);
	    	char buf[6];
	    	snprintf(buf, 6, "%d.%d", rSpeed/10, rSpeed%10);
	    	lcd_moveto(1, 12);
	    	lcd_stringout(buf);
	    	
	    	// Do we play a note?
		    if (maxspeed < rSpeed/10) {
		    	play_note();			
		    }
	    }
		// Do we play a note?

	    if (maxspeed <= time1) {
	    	if ( (oldSpeed != time1) ){ // only play if havent been played before    		
	    		play_note();
	    		oldSpeed = time1;
	    		oldMax = maxspeed;
	    	}			
	    }
	}

	return 0;
}

// Timer ISR

ISR(TIMER1_COMPA_vect){
	timeExceed = 1;
	TCCR1B &= ~((1 << CS12) | (1 << CS10)); 
}

// Pin Change Interrupt
ISR(PCINT1_vect) {
    input = PINC;
    a = (input & (1 << PC1)); // only getting a input
    b = (input & (1 << PC5)); // only taking in b input

    if (old_state == 0) { //00
		if (a != 0) { // 01
			new_state = 1;
			maxspeed++;
		}
		else if (b != 0) {
			new_state = 3;
			maxspeed--;
		}
	}
	else if (old_state == 1) { //01
		if (a == 0) { // 00
			new_state = 0;
			maxspeed--;
		}
		else if (b != 0) { // 11
			new_state = 2;
			maxspeed++;
		}
	}
	else if (old_state == 2) { //11
		if (a == 0) { // 10
			new_state = 3;
			maxspeed++;
		}
		else if (b == 0) { // 01
			new_state = 1;
			maxspeed--;
		}

	}
	else {   // old_state = 3 // 10
		if (a != 0) { // 11
			new_state = 2;
			maxspeed--;
		}
		else if (b == 0) { //00
			new_state = 0;
			maxspeed++;
		}
	}
	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
	    old_state = new_state;
	    changed = 1;
	}
}


// Receiving ISR
ISR(USART_RX_vect) {
	char ch;
	ch = UDR0; // Get the received charater

	if (ch == '<') {
		count = 0;
		received = 0;
		rBegin = 1;
		int i;
		for(i=0; i <5; i++) {
			buffer[i] = '\0';
		}
	}
	else if (rBegin == 1) {
		if( (ch== '>') && (count>0)){
			received = 1;
		}
		else if (isdigit(ch)) {
			if (count > 3) {
				received = 0;
			}
			else {
				buffer[count] = ch;
	 			count++;
			}
		}			
	}
	else {
		rBegin = 0;
	}
}
