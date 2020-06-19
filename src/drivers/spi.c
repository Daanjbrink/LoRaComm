#include <avr/io.h>

#include "spi.h"

void spiInit()
{
	DDRB  = (1<<MOSI) | (1<<SCK) | (1<<LORA); // Set all pins as output except MISO
	PORTB |= (1<<LORA); // Set chip select pins high
	SPCR  = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1); // SPI enable, Master mode
}

void spiSelectChip(uint8_t port)
{
	if (port == LCD || port == LORA) { // Port must be one of the chip select pins
		PORTB &= ~(1<<port); // Set only that pin low
	}
}

void spiUnselect()
{
	PORTB |= (1<<LCD) | (1<<LORA);
}

inline uint8_t spiTransfer(uint8_t data)
{
	SPDR = data; // Load the data register with the byte
	while (!(SPSR & (1<<SPIF))); // Wait until data is shifted out
	return SPDR; // Return incomming data
}

void spiWriteData(uint8_t *src, uint8_t length)
{
	while (length--)
		spiTransfer(*src++);
}

void spiReadData(uint8_t *dst, uint8_t length)
{
	while (length--)
		*dst++ = spiTransfer(0);
}