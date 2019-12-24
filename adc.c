#include <avr/io.h>

#include "adc.h"
#define MASK 0x0f; 

void adc_init(void) {
    // Initialize the ADC
	ADMUX &= ~(1 << REFS1);
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << ADLAR);
	ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0));
	ADCSRA |= (1 << ADEN);

}

unsigned char adc_sample(unsigned char channel) {
	ADMUX &= ~MASK;
	unsigned char copy_bits = channel & MASK;

	ADMUX |= copy_bits;
    
	ADCSRA |= (1 << ADSC); // set ADCS bit to 1

    // Convert an analog input and return the 8-bit result
	while ((ADCSRA & (1 << ADSC)) != 0) {
		// testing adsc each time
	}

	unsigned char result = ADCH;

	return result;
}