/*
* stopwatch.c
*
* Created: 5/19/2017 8:00:11 PM
* Author : Brian
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void InitTimer0(void)
{
	DDRD |= 1<<DDD6;
	
	// UNCOMMENT FOR TONE GENERATION
	TCCR0A |= (1 << COM0A0);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS01) | (1 << CS00);
	
	// UNCOMMENT FOR LIGHT SENSING
	//TCCR0A |= (1 << COM0A1);
	//TCCR0A |= (1 << WGM02) | (1 << WGM01) | (1 << WGM00);
	//TCCR0B |= (1 << CS01) | (1 << CS00);
}

void init_ADC(void)
{
	DDRC &= 0x00;
	// ADLAR set to 1 ? left adjusted result in ADCH
	// MUX3:0 set to 0001 ? input voltage at ADC1
	ADMUX = (1<<MUX0) | (1<<ADLAR);
	// ADEN set to 1 ? enables the ADC circuitry
	// ADPS2:0 set to 111 ? prescalar set to 128 (104us per conversion)
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

unsigned char GetADCResult(void)
{
	// Start A to D conversion
	ADCSRA |= (1<<ADSC);
	// Wait for conversion to complete
	while (ADCSRA & 1<<ADSC);
	return (ADCH);
}

int main(void)
{
	InitTimer0();
	init_ADC();
	
	while(1)
	{
		OCR0A = GetADCResult();
	}
	
	return 0;
}