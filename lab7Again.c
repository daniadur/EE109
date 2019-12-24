#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include <stdlib.h>

void variable_delay_us(int);
void play_note(unsigned short);


// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
unsigned short frequency[8] =
    { 262, 294, 330, 349, 392, 440, 494, 523 };

volatile unsigned char new_state, old_state;
volatile unsigned char changed = 0;  // Flag for state change
volatile int count = 0;		// Count to display
volatile unsigned char a, b;
volatile unsigned char register1;


int main(void) {

// Initialize DDR and PORT registers and LCD
    //output
    DDRB |= (1 << 4);
    //pull up resistors for A1 and A5
    //DDRC &= ~( (1 << 1) | (1 << 5) );
    PORTC |= ((1 << PC1) | (1 << PC5));

    // Initialize output to 0
    PORTB &= ~(1 << 4);
    lcd_init();
    // Write a spash screen to the LCD
    lcd_splash(7);
    //enable interrupts
    PCICR |= (1<<PCIE1);
    PCMSK1 |= ((1 << PCINT9) | (1 << PCINT13));

    //enable global interrupts
    sei();

    

    //geck both variables at teh same time
    register1 = PINC;
    a = (register1 & (1 << PC1));
    b = (register1 & (1 << PC5));

    // Read the A and B inputs to determine the intial state
    // Warning: Do NOT read A and B separately.  You should read BOTH inputs
    // at the same time, then determine the A and B values from that value.

    if (!b && !a)
	   old_state = 0;
    else if (!b && a)
	   old_state = 1;
    else if (b && !a)
	   old_state = 2;
    else
	   old_state = 3;

    new_state = old_state;

    char buf[16];
    snprintf(buf, 16, "Count:  %d", count);
    lcd_stringout(buf);


    while (1) {                 // Loop forever
  
     if (changed) { // Did state change?
         lcd_stringout("here");
         changed = 0;        // Reset changed flag
         char buf[40];
         snprintf(buf, 40, "Count:  %d", count);
    	   // Output count to LCD
         lcd_writecommand(1);
         lcd_stringout(buf);
    	   // Do we play a note?
    	   if ((count % 8) == 0) {
  		     // Determine which note (0-7) to play
           unsigned int note = (abs(count) % 64) / 8;
    		   // Find the frequency of the note
           unsigned short freq = frequency[note];
    		   // Call play_note and pass it the frequency
           play_note(freq);
    	   }
      }
    }
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(unsigned short freq)
{
    unsigned long period;

    period = 1000000 / freq;      // Period of note in microseconds

    while (freq--) {
      // Make PB4 high
      PORTB |= (1 << PB4);
      // Use variable_delay_us to delay for half the period
      variable_delay_us(period/2);
      // Make PB4 low
      PORTB &= ~(1 << PB4);
      // Delay for half the period again
      variable_delay_us(period/2);
    }
}

/*
    variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}

//run program inside interrupt if a bit inside PINC changes
ISR(PCINT1_vect){
  register1 = PINC;
  a = (register1 & (1 << PC1));
  b = (register1 & (1 << PC5));
  changed = 1;
 // For each state, examine the two input bits to see if state
 // has changed, and if so set "new_state" to the new state,
 // and adjust the count value.
 if (old_state == 0) {
   // Handle A and B inputs for state 0
   if(a){
     new_state = 1;
     count++;
   }
   else if(b){
     new_state = 2;
     count--;
  }
 }
 else if (old_state == 1) {
     // Handle A and B inputs for state 1
     if(!a){
       new_state = 0;
       count--;
     }
     else if(b){
       new_state = 3;
       count++;
    }
 }
 else if (old_state == 2) {
     // Handle A and B inputs for state 2
     if(a){
       new_state = 3;
       count--;
     }
     else if(!b){
       new_state = 0;
       count++;
    }

 }
 else {   // old_state = 3

     // Handle A and B inputs for state 3
     if(!a){
       new_state = 2;
       count++;
     }
     else if(!b){
       new_state = 1;
       count--;
    }
 }

 // If state changed, update the value of old_state,
 // and set a flag that the state has changed.
 if (new_state != old_state) {
     changed = 1;
     old_state = new_state;
 }

}