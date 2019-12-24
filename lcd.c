#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"                // Declarations of the LCD functions
#include "speedtrap.h"
/* Define a couple of masks for the bits in Port B and Port D */
#define DATA_BITS ((1 << PD7)|(1 << PD6)|(1 << PD5)|(1 << PD4))
#define CTRL_BITS ((1 << PB1)|(1 << PB0))

void lcd_writenibble(unsigned char);

void lcd_init(void) {
    DDRB |= ( (1 << 0) | (1 << 1) ); // Set the DDR register bits for ports B and D
    DDRD |= ( (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) );
     // Take care not to affect any unnecessary bits

    _delay_ms(15);              // Delay at least 15ms

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_ms(5);               // Delay at least 4msec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_us(120);             // Delay at least 100usec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011, no delay needed

    lcd_writenibble(0x20);      // Use lcd_writenibble to send 0b0010
    _delay_ms(2);               // Delay at least 2ms
    
    lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

    lcd_writecommand(0x0f);     // Display and cursor on
}

void lcd_moveto(unsigned char row, unsigned char col) {
    unsigned char pos;
    if(row == 0) {
        pos = 0x80 + col;       // 1st row locations start at 0x80
    }
    else {
        pos = 0xc0 + col;       // 2nd row locations start at 0xc0
    }
    lcd_writecommand(pos);      // Send command
}

void lcd_stringout(char *str) {
    int i = 0;
    while (str[i] != '\0') {    // Loop until next charater is NULL byte
        lcd_writedata(str[i]);  // Send the character
        i++;
    }
}

void lcd_writecommand(unsigned char cmd) {
    /* Clear PB0 to 0 for a command transfer */
  PORTB &= ~(1 << PB0);
    /* Call lcd_writenibble to send UPPER four bits of "cmd" argument */
  lcd_writenibble(cmd);
    /* Call lcd_writenibble to send LOWER four bits of "cmd" argument */
  lcd_writenibble(cmd<<4); // move lower bits to top
    /* Delay 2ms */
  _delay_ms(2);
}

void lcd_writedata(unsigned char dat) {
    /* Set PB0 to 1 for a data transfer */
  PORTB |= (1 << PB0);
      /* Call lcd_writenibble to send UPPER four bits of "cmd" argument */
  lcd_writenibble(dat);
    /* Call lcd_writenibble to send LOWER four bits of "cmd" argument */
  lcd_writenibble(dat<<4); // move lower bits to top
    /* Delay 2ms */
  _delay_ms(2);
}

void lcd_writenibble(unsigned char lcdbits) {
    /* Load PORTD, bits 7-4 with bits 7-4 of "lcdbits" */
  PORTD &= 0x0f; // clear upper half
  PORTD |= (lcdbits & (0xf0)); // fill with lcdbits

 /* Make E signal (PB1) go to 1 and back to 0 */
  PORTB |= (1 << PB1); // 1
  PORTB &= ~(1 << PB1); // 0
}

void lcd_splash(void) {
  // write name
  lcd_writecommand(1);
  lcd_moveto(0, 2);
  lcd_stringout("dania duran");

  // center lab number
  lcd_moveto(1,0);
  lcd_stringout("Final Project <3");
  _delay_ms(1000);

  // clear screen
  lcd_writecommand(1);
}