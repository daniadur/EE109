#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "speedtrap.h"
// Serial communications functions and variables
void serial_init(void);
void serial_stringout(char *);
void serial_txchar(char);

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register

void serial_init(void) {
	// Enable interrupts
    UCSR0B |= (1 << RXCIE0);    // Enable receiver interrupts
    UBRR0 = MYUBRR;             // Set baud rate
    UCSR0C = (3 << UCSZ00);               // Async., no parity,
                                          // 1 stop bit, 8 data bits
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);  // Enable RX and TX

    DDRC |= (1 << PC3); // Setting AC3 as output
    PORTC &= ~(1 << PC3); // enable the buffer
}

void serial_txchar(char ch) {
    //waits until char has been sent
    while ((UCSR0A & (1<<UDRE0)) == 0){
      // wait
    }
    UDR0 = ch;
}

void serial_stringout(char *s) {
    // Call serial_txchar in loop to send a string of 6 char
    int i = 0;
    while (s[i] != '\0') {
        serial_txchar(s[i]);
        i++;
    }
    serial_txchar('\0');
}