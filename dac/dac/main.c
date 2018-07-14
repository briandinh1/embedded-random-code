/*
 * dac.c
 *
 * Created: 5/31/2017 3:10:43 PM
 * Author : Brian
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void UART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void UART_Transmit(unsigned char data)
{
	while( !(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
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

void SPI_MasterInit(void)
{
	// Set MOSI and SCK output, all others input
	DDRB |= (1<<PORTB3)|(1<<PORTB5);
	// Enable SPI, Master, clock = fosc/16
	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}


void spi_send(uint16_t data)
{
	uint8_t config = 0b00111111;
	uint8_t upper_byte = (config & 0xF0)|(0x0F & (data>>8));
	uint8_t lower_byte = (data & 0xFF);

	DDRB &= ~(1<<PORTB2);
	SPDR = upper_byte;
	while(!(SPSR & (1<<SPIF)));
	SPDR = lower_byte;
	while(!(SPSR & (1<<SPIF)));
	DDRB |= (1<<PORTB2);
}

int main(void)
{
	init_ADC();
	SPI_MasterInit();
	UART_Init(MYUBRR);
	
    while (1) 
    {
		uint16_t c = GetADCResult();
		spi_send(c);
    }
}
