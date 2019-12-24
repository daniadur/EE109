/********************************************
 *
 *  Name: Dania duran
 *  Email:daniadur@usc.edu
 *  Section: 12:30 TTh
 *  Assignment: Lab 8 - Digital Stopwatch
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "adc.h"

volatile char flag = 0, screen = 0;
volatile unsigned int num1 = 0, num2 = 0, num3 = 0;

void isPressed(void);

int main(void) {
    // timer initialization
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = 6250;// assume preload = 256
    //TCCR1B |= (1 << CS12);
    sei(); // enable global interrupts

    // Initialize the LCD
	lcd_init();

    // Initialize the ADC
	adc_init();

    // Write splash screen
	lcd_splash(8);

    // Variables
    unsigned char adc_result;
    int state = 0, oldstate = 0;// stopped
    int input = 0;

    // Initialization
    lcd_moveto(0,0);
    char buf[5]; 
    snprintf(buf, 5, "%d%d.%d", num1, num2, num3);
    lcd_stringout(buf);
    lcd_moveto(0,2);
    lcd_stringout(".");

    while (1) { // Loop forever

        // sample the adc
        adc_result = adc_sample(0);

        if (adc_result < 230) {
            isPressed();
            // Button Presses
            if (adc_result < 65 && adc_result > 40) {// start stop
                input = 1;
            }
            else if (adc_result > 85 && adc_result < 120) {//Lap_Reset Button Pressed
                input = 2;
            }
        }

        if (oldstate == 0) {
            if (input == 1) {
                TCCR1B |= (1 << CS12); // start timer
                state = 1;
                screen = 1;
            }
            else if (input == 2) {
                state = 0;
                screen = 1;
                num1 = 0;
                num2 = 0;
                num3 = 0;
            }
       }
        else if (oldstate == 1) {
            if (input == 1) {
                state = 0;
                screen = 0;
                TCCR1B &= ~(1 << CS12); // stop timer
            }
            else if (input == 2) { // lap reset
                state = 2;
                screen = 0;
            }
        }
        else if (oldstate == 2) {
            if ( (input == 1) || (input == 2) ) {
                state = 1;
                screen = 1;
                TCCR1B |= (1 << CS12);
            }
        }
        

        if (screen) {
            if (flag) {
                flag = 0;
                lcd_moveto(0,0);
                char buf[5];
                snprintf(buf, 5, "%d%d.%d", num1, num2, num3);
                lcd_stringout(buf);
            }
        }
        if (state != oldstate) {
            oldstate = state;
            input = 0;
        }
    }

    return 0;   /* never reached */
}

void isPressed(void) {
    _delay_ms(200);
}

ISR(TIMER1_COMPA_vect){
    if (num3 < 9) {
        num3++;
    }
    else if (num3 == 9) {
        if (num2 < 9) {
            num3 = 0;
            num2++;
        }
        else {
            num2 = 0;
            num1++;
        }
        if (num1 == 5) {
            if (num2 == 9) {
                num1 = 0;
                num2 = 0;
                num3 = 0;
            }
        }

    }

    flag = 1;
}
