#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "encoder.h"
#include "speedtrap.h"

#define FREQ 392
#define DELAY (1000000 / (2*FREQ))

void play_note(void);
void encoder_init(void);
void variable_delay_us(void);

volatile int timeDelay = 0;

void encoder_init(void) {

	// Timer Initialization
	TCCR0B |= (1 << WGM02);
    TIMSK0 |= (1 << OCIE0A);
    TCNT0 = 0;
    OCR0A = 15625;
    
	// Initialize DDR and PORT registers and LCD
    DDRB |= (1 << PB4);
    PORTC |= ( (1 << PC1) | (1 << PC5) ); // enable pull up
    PORTB &= ~(1 << PB4); // set bit 4 as output

    // interrupt initialization
   	PCICR |= (1 << PCIE1); // enable pin change interrupt
   	PCMSK1 |= ( (1 << PCINT9) | (PCINT13) ); // enable interrupt	
}

void play_note(void) {
	unsigned short freq = FREQ;
    unsigned long period;
    period = 1000000 / freq;      // Period of note in microseconds
    /*
    while (freq--) {
		PORTB |= (1 << PB4); // Make PB4 high
		variable_delay_us(); // Use variable_delay_us to delay for half the period
		PORTB &= ~(1 << PB4); // Make PB4 low
		variable_delay_us(); // Delay for half the period again
	}
	*/

    TCNT0 = 0; // clear register
    TCCR0B &= ~((1 << CS02) | (1 << CS00)); // set timer
	while (freq--) {
		lcd_moveto(1, 12);
		char buf[6];

		snprintf(buf,6, "%d %d", TCNT0, timeDelay);
		lcd_stringout(buf);
		if (timeDelay == 1) {
			PORTB |= (1 << PB4); // Make PB4 high
		}
		else if (timeDelay == 2) {
			PORTB &= ~(1 << PB4); // Make PB4 low
		}
	}


}


ISR(TIMER0_COMPA_vect){
	if (timeDelay == 1) {
		timeDelay = 2;
	}
	else {
		timeDelay = 1;
	}
	
	TCCR0B &= ~((1 << CS02) | (1 << CS00)); 
}
// variable_delay_us - Delay a variable number of microseconds
void variable_delay_us(void) {
    int i = (DELAY + 5) / 10;

    while (i--)
        _delay_us(10);
}