/********************************************
 *
 *  Name: Dania Duran 
 *  Email: daniadur@usc.edu
 *  Section: 12:30 F
 *  Assignment: Lab 9 - Serial Communications
 *
 ********************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "adc.h"

// Serial communications functions and variables
void serial_init(unsigned short);
void serial_stringout(char *);
void serial_txchar(char);

#define FOSC 16000000           // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0 register

// ADC functions and variables
void isPressed(void);

#define ADC_CHAN 0              // Buttons use ADC channel 0

char *messages[] = {
    "Hello           ",
    "How are you?    ",
    "That's my type! ",
    "A lovely day!   ",
    "ENGINEERINGSUCKS",
    " i love your mom",
    "Basic hoes pised",
    "Bitch please!   ",
    "What am I doing?",
    "Bonjourno!      ",
};

//global variables
volatile int count = 0;
volatile char buffer[17];
volatile char received = 0;
volatile int cstate = 0;
volatile int nstate = 0;
volatile int adc_result;

int main(void) {

    // Initialize the modules and LCD

    serial_init(MYUBRR);
    lcd_init();
    adc_init();

    // Enable interrupts
    UCSR0B |= (1 << RXCIE0);    // Enable receiver interrupts
    sei();                      // Enable interrupts
/*
    UBRR0 = MYUBRR;             // Set baud rate
    UCSR0C = (3 << UCSZ00);               // Async., no parity,
                                          // 1 stop bit, 8 data bits
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);  // Enable RX and TX
*/

    // Show the splash screen
    lcd_splash(9);

    //show first message
    lcd_moveto(0,0);
    lcd_stringout(messages[nstate]);

    //initialize states
    cstate = 0;
    nstate = 0;


    while (1) {                 // Loop forever

        // Get an ADC sample
        adc_result = adc_sample(ADC_CHAN);
        //delay if button has been pressed
        isPressed();

        // Up button pressed?
        if(adc_result < 65 && adc_result > 40){
          //loop around if at the beginning of messages
          if(nstate == 0)
            nstate = 9;
          else
            nstate = cstate - 1;
        }

        // Down button pressed?
        else if(adc_result > 85 && adc_result < 120){
          //loop around if at the beginning of messages
          if(nstate == 9)
            nstate = 0;
          else
            nstate = cstate + 1;
        }

        //print onto lcd if state change
        if(nstate != cstate){
          //nstate = nstate % 11;
          lcd_moveto(0,0);
          lcd_stringout(messages[nstate]);
        }

        // Select button pressed
        else if(adc_result < 255 && adc_result > 180){
          lcd_moveto(0,0);
          serial_stringout(messages[cstate]);           //sends message to be transmitted

        }
        
        char test[5];
        snprintf(test, 5, "%d", received);
        lcd_stringout(test);
        // Message received from remote device
        if(received==1){
          //print out received message

          lcd_moveto(1,0);
          lcd_stringout("hi");
          //lcd_stringout("I received");
          // char buf[17];
          // snprintf(buf, 17, "%s", buffer);
          // lcd_stringout(buf);
          //count = 0;
          //reset receive flag
          received = 0;
        }

        cstate = nstate;
    }
}

/* ----------------------------------------------------------------------- */

void serial_init(unsigned short ubrr_value)
{
    // Set up USART0 registers
    // Enable tri-state


    UBRR0 = MYUBRR;             // Set baud rate
    UCSR0C = (3 << UCSZ00);               // Async., no parity,
                                          // 1 stop bit, 8 data bits
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);  // Enable RX and TX

    DDRD |= (1 << 3);
    PORTD &= ~(1 << 3);
}

void serial_txchar(char ch)
{
    //waits until char has been sent
    while ((UCSR0A & (1<<UDRE0)) == 0){
      // wait
    }
    UDR0 = ch;
}

void serial_stringout(char *s)
{
    // Call serial_txchar in loop to send a string of 16 char
    int i;
    for(i = 0; i < 16 ; i++){
  //int i =0;
    //while (s[i] != '\0') {
      //send one char at a time
      serial_txchar(s[i]);

    }
    //s[16] = '\0';

}

ISR(USART_RX_vect)
{
  char ch;

  ch = UDR0;                  // Get the received charater

  // Store in buffer and increment count
  buffer[count] = ch;
  count++;
  //if 16 char have been received reset count and set flag to true
  if(count == 16){
    buffer[16] = '\0';
    count = 0;
    received = 1;
  }
}

void isPressed(void) {
    _delay_ms(200);
}